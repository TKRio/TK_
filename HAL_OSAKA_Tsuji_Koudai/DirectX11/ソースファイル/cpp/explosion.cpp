//*****************************************************************************
//!	@file	explosion.cpp
//!	@brief	
//!	@note	爆発処理のための関数群(1つの頂点バッファにまとめる)
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<windows.h>
#include	"explosion.h"
#include	"shader.h"
#include	"DX11util.h"
#include	"DX11Settransform.h"

//-----------------------------------------------------------------------------
//	グローバル変数
//-----------------------------------------------------------------------------
MyVertex	*g_LocalVertex = nullptr;			// 頂点情報
int			g_NumVertex=0;						// 頂点数
MyConnect	*g_Connect = nullptr;				// 結線情報
int			g_NumFace=0;						// 面数
MyTriangle	*g_pTriangle = nullptr;				// トライアングルの先

const float deceleration = 0.05f;				// 爆発の破片が飛び散る速度
const double gravity = 0.0f;					//重力
const double deltaTime = 0.0001;				//微小時間
double time;
double moveV = 0;

ID3D11ShaderResourceView*  g_srv;				// シェーダーリソースビュー

ID3D11VertexShader* g_pVertexShader=nullptr;	// 頂点シェーダー
ID3D11PixelShader*  g_pPixelShader=nullptr;		// ピクセルシェーダー
ID3D11InputLayout*  g_pVertexLayout=nullptr;	// 頂点レイアウト

ID3D11Buffer*		g_pVertexBuffer = nullptr;	// 頂点バッファ

MyVertex*			g_firstpvertex = nullptr;	
//==============================================================================
//!	@fn		InitTriangleVertexBuffer
//!	@brief	ばらばらな３角形頂点情報をまとめた頂点バッファを作成する
//!	@param　MyTriangle構造体配列　　	
//!	@retval	なし
//==============================================================================
void InitTriangleVertexBuffer(MyTriangle *mytriangle) {

	MyVertex* svpv;

	// 古いのが残っていたら削除
	if (g_firstpvertex){
		delete[] g_firstpvertex;
	}

	g_firstpvertex = new MyVertex [g_NumFace * 3];
	svpv = g_firstpvertex;
	for (int i = 0; i < g_NumFace; i++) {
		svpv->Pos = mytriangle[i].Vertex[0].Pos;
		svpv->col = mytriangle[i].Vertex[0].col;
		svpv->tu = mytriangle[i].Vertex[0].tu;
		svpv->tv = mytriangle[i].Vertex[0].tv;

		svpv++;
		svpv->Pos = mytriangle[i].Vertex[1].Pos;
		svpv->col = mytriangle[i].Vertex[1].col;
		svpv->tu = mytriangle[i].Vertex[1].tu;
		svpv->tv = mytriangle[i].Vertex[1].tv;

		svpv++;
		svpv->Pos = mytriangle[i].Vertex[2].Pos;
		svpv->col = mytriangle[i].Vertex[2].col;
		svpv->tu = mytriangle[i].Vertex[2].tu;
		svpv->tv = mytriangle[i].Vertex[2].tv;
		svpv++;
	}

	// 頂点バッファ作成(CPU側で書き込み可能)
	bool sts = CreateVertexBufferWrite(
		GetDX11Device(),
		sizeof(MyVertex),		// １頂点当たりバイト数
		g_NumFace * 3,			// 頂点数
		g_firstpvertex,			// 頂点データ格納メモリ先頭アドレス
		&g_pVertexBuffer);		// 頂点バッファ
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(vertex buffer) error", "Error", MB_OK);
		return;
	}
}

//==============================================================================
//!	@fn		TriangleTransforms
//!	@brief	ばらばらな３角形頂点情報（ローカル座標）をワールド座標系に変換
//!	@param　ワールド変換行列(入力)　　	
//!	@retval	なし
//==============================================================================
void TriangleTransforms(const DirectX::XMFLOAT4X4& mat)
{
	int					i;
	MyVertex			*TransformWork;			// ワールド座標系に変換するためのワーク
	DirectX::XMFLOAT3	temp;

	int				idx0, idx1, idx2;
	DirectX::XMFLOAT3		normal;
	DirectX::XMFLOAT3		p0p1;
	DirectX::XMFLOAT3		p0p2;

	// 生成された三角形情報があれば削除する
	if (g_pTriangle != nullptr){
		delete[] g_pTriangle;
		g_pTriangle = nullptr;
	}

	TransformWork = new MyVertex[g_NumVertex];		// 座標変換用ワークを頂点数分生成する

	// 頂点をすべて取り出しワールド座標に変換する
	for (i = 0; i<g_NumVertex; i++){
		TransformWork[i] = g_LocalVertex[i];		// ローカル座標を座標変換用ワークにセット

		temp = g_LocalVertex[i].Pos;

		DX11Vec3MulMatrix(temp, temp, mat);			//ここで外積を求めている

		TransformWork[i].Pos = temp;				// 座標値のみワールド座標で書き換え
	}

	g_pTriangle = new MyTriangle[g_NumFace];			// 三角形の面数分オブジェクト生成

	for (i = 0; i<g_NumFace; i++){						// ３角形ポリゴン数分ループ
		idx0 = g_Connect[i].idx[0];
		idx1 = g_Connect[i].idx[1];
		idx2 = g_Connect[i].idx[2];

		g_pTriangle[i].Vertex[0] = TransformWork[idx0];
		g_pTriangle[i].Vertex[0].col = DirectX::XMFLOAT4(1,1,1,1);		// ディフューズ値をセットする。（Ｘファイルに含まれていない為）

		g_pTriangle[i].Vertex[1] = TransformWork[idx1];
		g_pTriangle[i].Vertex[1].col = DirectX::XMFLOAT4(1, 1, 1, 1);		// ディフューズ値をセットする。（Ｘファイルに含まれていない為）

		g_pTriangle[i].Vertex[2] = TransformWork[idx2];
		g_pTriangle[i].Vertex[2].col = DirectX::XMFLOAT4(1, 1, 1, 1);		// ディフューズ値をセットする。（Ｘファイルに含まれていない為）

		p0p1.x = g_pTriangle[i].Vertex[1].Pos.x - g_pTriangle[i].Vertex[0].Pos.x;
		p0p1.y = g_pTriangle[i].Vertex[1].Pos.y - g_pTriangle[i].Vertex[0].Pos.y;
		p0p1.z = g_pTriangle[i].Vertex[1].Pos.z - g_pTriangle[i].Vertex[0].Pos.z;

		p0p2.x = g_pTriangle[i].Vertex[2].Pos.x - g_pTriangle[i].Vertex[0].Pos.x;
		p0p2.y = g_pTriangle[i].Vertex[2].Pos.y - g_pTriangle[i].Vertex[0].Pos.y;
		p0p2.z = g_pTriangle[i].Vertex[2].Pos.z - g_pTriangle[i].Vertex[0].Pos.z;

		// 法線情報計算
		DX11Vec3Cross(normal, p0p1, p0p2);			// 外積を計算
		DX11Vec3Normalize(normal,normal);			// ノーマライズ
		g_pTriangle[i].nx = normal.x;				// 法線情報セット
		g_pTriangle[i].ny = normal.y;
		g_pTriangle[i].nz = normal.z;

		// 重心計算
		g_pTriangle[i].cx = (g_pTriangle[i].Vertex[0].Pos.x + g_pTriangle[i].Vertex[1].Pos.x + g_pTriangle[i].Vertex[2].Pos.x) / 3;
		g_pTriangle[i].cy = (g_pTriangle[i].Vertex[0].Pos.y + g_pTriangle[i].Vertex[1].Pos.y + g_pTriangle[i].Vertex[2].Pos.y) / 3;
		g_pTriangle[i].cz = (g_pTriangle[i].Vertex[0].Pos.z + g_pTriangle[i].Vertex[1].Pos.z + g_pTriangle[i].Vertex[2].Pos.z) / 3;

		// 重心座標を原点を基準とした座標に変換する
		g_pTriangle[i].Vertex[0].Pos.x -= g_pTriangle[i].cx;
		g_pTriangle[i].Vertex[0].Pos.y -= g_pTriangle[i].cy;
		g_pTriangle[i].Vertex[0].Pos.z -= g_pTriangle[i].cz;

		g_pTriangle[i].Vertex[1].Pos.x -= g_pTriangle[i].cx;
		g_pTriangle[i].Vertex[1].Pos.y -= g_pTriangle[i].cy;
		g_pTriangle[i].Vertex[1].Pos.z -= g_pTriangle[i].cz;

		g_pTriangle[i].Vertex[2].Pos.x -= g_pTriangle[i].cx;
		g_pTriangle[i].Vertex[2].Pos.y -= g_pTriangle[i].cy;
		g_pTriangle[i].Vertex[2].Pos.z -= g_pTriangle[i].cz;
	}

	InitTriangleVertexBuffer(g_pTriangle);

	delete[] TransformWork;
	return;
}

//==============================================================================
//!	@fn		XfileToTriangle
//!	@brief	Ｘファイルをばらばらな３角形プリミティブにばらす
//!	@param　メッシュオブジェクトポインタ(入力)　　	
//!	@param　デバイスオブジェクト(入力)　　	
//!	@retval	なし
//==============================================================================
void PMDToTriangle(Model::cPmd* pmdData)
{
	int							length;			// 頂点フォーマット長（バイト）

	// 爆発用のメッシュを作成

	g_NumVertex = pmdData->GetVertexCount();			// 頂点数を取得

	g_NumFace = pmdData->GetModelData()->faceVertCount / 3;		// 面数をゲット

	length = sizeof(MyVertex);

	g_LocalVertex = new MyVertex[g_NumVertex];

	// 頂点情報をコピー
	for (int i = 0; i < g_NumVertex; i++) {
		g_LocalVertex[i].Pos = pmdData->GetVertexData()->position;		// 座標取り出し
		g_LocalVertex[i].tu = pmdData->GetVertexData()->uv.x;		// UV座標取り出し
		g_LocalVertex[i].tv = pmdData->GetVertexData()->uv.y;
		g_LocalVertex[i].col = DirectX::XMFLOAT4(1,1,1,1);
	}

	g_Connect = new MyConnect[g_NumFace];	// 面数分の結線情報を取得

	unsigned int idx = 0;
	for (int i = 0; i<g_NumFace; i++){		// ３角形ポリゴン数分ループ
		g_Connect[i].idx[0] = pmdData->GetIndexData()[idx++];
		g_Connect[i].idx[1] = pmdData->GetIndexData()[idx++];
		g_Connect[i].idx[2] = pmdData->GetIndexData()[idx++];
	}

	return;
}

//==============================================================================
//!	@fn		ExplosionInit
//!	@brief	爆発のための初期処理を行う
//!	@param　DatFileLoaderポインタ(入力)　　	
//!	@retval	なし
//==============================================================================
void ExplosionInit(Model::cPmd* pmdData){

	// Xファイルをバラバラな３角形に分割する
	PMDToTriangle(pmdData);
	bool sts = CreatetSRVfromTGAFile("assets/koa_1.tga",
		GetDX11Device(),
		&g_srv);
	if (!sts) {
		MessageBox(NULL, "CreatetSRVfromTGAFile", "Error", MB_OK);
	}

	// 頂点データの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	unsigned int numElements = ARRAYSIZE(layout);

	// 頂点シェーダーオブジェクトを生成、同時に頂点レイアウトも生成
	sts = CreateVertexShader(GetDX11Device(),
		"shader/vs.fx",
		"main",
		"vs_5_0",
		layout,
		numElements,
		&g_pVertexShader,
		&g_pVertexLayout);
	if (!sts) {
		MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
		return;
	}

	// ピクセルシェーダーを生成
	sts = CreatePixelShader(			// ピクセルシェーダーオブジェクトを生成
		GetDX11Device(),				// デバイスオブジェクト
		"shader/ps.fx",
		"main",
		"ps_5_0",
		&g_pPixelShader);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return;
	}
}

//==============================================================================
//!	@fn		ExplosionUpdate
//!	@brief	爆発の更新処理
//!	@param　　　	
//!	@retval	なし
//==============================================================================
void ExplosionUpdate(){

	MyVertex* pv = new MyVertex [g_NumFace*3];

	// ３角形の数分ループする
	for (int i = 0; i <g_NumFace; i++) {
		XMFLOAT3 angle;
		XMFLOAT3 trans = { 0,0,0 };

		angle.x = g_pTriangle[i].xangle;
		angle.y = g_pTriangle[i].yangle;
		angle.z = g_pTriangle[i].zangle;
		DX11MakeWorldMatrix(g_pTriangle[i].mat, angle, trans);		// 回転角度と平行移動量から行列を作成する

		// 位置座標を重心位置に変更する
		g_pTriangle[i].mat._41 = g_pTriangle[i].cx;
		g_pTriangle[i].mat._42 = g_pTriangle[i].cy;
		g_pTriangle[i].mat._43 = g_pTriangle[i].cz;

		// ３次元座標に行列を掛ける
		DX11Vec3MulMatrix(pv[i * 3 + 0].Pos, g_firstpvertex[i * 3 + 0].Pos, g_pTriangle[i].mat);
		DX11Vec3MulMatrix(pv[i * 3 + 1].Pos, g_firstpvertex[i * 3 + 1].Pos, g_pTriangle[i].mat);
		DX11Vec3MulMatrix(pv[i * 3 + 2].Pos, g_firstpvertex[i * 3 + 2].Pos, g_pTriangle[i].mat);

		// ＵＶ座標をセットする
		pv[i * 3 + 0].tu = g_firstpvertex[i * 3 + 0].tu;
		pv[i * 3 + 0].tv = g_firstpvertex[i * 3 + 0].tv;
		pv[i * 3 + 1].tu = g_firstpvertex[i * 3 + 1].tu;
		pv[i * 3 + 1].tv = g_firstpvertex[i * 3 + 1].tv;
		pv[i * 3 + 2].tu = g_firstpvertex[i * 3 + 2].tu;
		pv[i * 3 + 2].tv = g_firstpvertex[i * 3 + 2].tv;

		// だんだん透明にしていく
		for (int k = 0; k < 3; k++) {
			g_pTriangle[i].Vertex[k].col.w -= 0.01f;
			if (g_pTriangle[i].Vertex[k].col.w < 0.0f) {
				g_pTriangle[i].Vertex[k].col.w = 0.0f;
			}
		}

		// 頂点カラーをセット
		pv[i * 3 + 0].col = g_pTriangle[i].Vertex[0].col;
		pv[i * 3 + 1].col = g_pTriangle[i].Vertex[1].col;
		pv[i * 3 + 2].col = g_pTriangle[i].Vertex[2].col;
		///////////////////////////////////////////////////////////////////////////////////////////
		// ここからコードを追加
		// 回転角度の足し算
		g_pTriangle[i].xangle += g_pTriangle[i].dxa;
		g_pTriangle[i].yangle += g_pTriangle[i].dya;
		g_pTriangle[i].zangle += g_pTriangle[i].dza;

		// 重心座標を法線方向に足し算
		g_pTriangle[i].cx += g_pTriangle[i].nx*deceleration;
		g_pTriangle[i].cy += g_pTriangle[i].ny*deceleration;
		g_pTriangle[i].cz += g_pTriangle[i].nz*deceleration;

		//破片の重力加速度
		for (int j = 0; j < 10; j++) {
			time += deltaTime;
			moveV += deltaTime * gravity;
			g_pTriangle[i].cy -= deltaTime * moveV;
		}
		// ここまでコードを追加
		///////////////////////////////////////////////////////////////////////////////////////////
	}

	D3D11_MAPPED_SUBRESOURCE pData;

	HRESULT hr = GetDX11DeviceContext()->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	if (SUCCEEDED(hr)) {
		memcpy_s(pData.pData, pData.RowPitch, (void*)(pv), sizeof(MyVertex) * g_NumFace*3);
		GetDX11DeviceContext()->Unmap(g_pVertexBuffer, 0);
	}

	delete[] pv;
}

//==============================================================================
//!	@fn		ExplosionDraw
//!	@brief	爆発を描画する
//!	@param　デバイスオブジェクト(入力)　　	
//!	@retval	なし
//==============================================================================
void ExplosionDraw(){

	XMFLOAT4X4 mtx;

	DX11MatrixIdentity(mtx);

	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, mtx);

	// 頂点バッファをセットする
	unsigned int stride = sizeof(MyVertex);
	unsigned  offset = 0;
	GetDX11DeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	GetDX11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		// トポロジーをセット（旧プリミティブタイプ）

																							// 頂点レイアウトセット
	GetDX11DeviceContext()->IASetInputLayout(g_pVertexLayout);

	GetDX11DeviceContext()->VSSetShader(g_pVertexShader, nullptr, 0);	// 頂点シェーダーをセット
	GetDX11DeviceContext()->PSSetShader(g_pPixelShader, nullptr, 0);	// ピクセルシェーダーをセット

	GetDX11DeviceContext()->PSSetShaderResources(0, 1, &g_srv);		// シェーダーリソースビューをピクセルシェーダーへセット

	GetDX11DeviceContext()->Draw(g_NumFace * 3, 0);
}


//==============================================================================
//!	@fn		ExplosionExit
//!	@brief	爆発処理の後始末
//!	@param　デバイスオブジェクト(入力)　　	
//!	@retval	なし
//==============================================================================
void ExplosionExit(){

	// 生成された三角形情報があれば削除する
	if (g_pTriangle != nullptr){
		delete[] g_pTriangle;
		g_pTriangle = nullptr;
	}

	if(g_Connect != nullptr){
		delete [] g_Connect;
		g_Connect = nullptr;
	}

	if(g_LocalVertex != nullptr){
		delete[] g_LocalVertex;
		g_LocalVertex = nullptr;
	}

	if (g_srv) {
		g_srv->Release();
		g_srv = nullptr;
	}

	if (g_pVertexBuffer) {
		g_pVertexBuffer->Release();
		g_pVertexBuffer = nullptr;
	}

	if (g_firstpvertex) {
		delete[] g_firstpvertex;
	}
}

//******************************************************************************
//	End of file.
//******************************************************************************

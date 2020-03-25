//*****************************************************************************
//!	@file	Model.cpp
//!	@brief	PMD&PMXを読み込んでモデルを描画する
//!	@note
//!	@author	
//*****************************************************************************
#include	<windows.h>
#include	"Shader.h"
#include	"CDirectxGraphics.h"
#include	"DX11Settransform.h"
#include	"Model.h"
#include	<string>

bool MMDBreaker::PMDModel::Init(const char* fileName, const char* vsfile, const char* psfile) {
	bool sts;
	modelDate = new PMDData(fileName);		//モデルデータの取得	
	if (modelDate == nullptr) {
		MessageBox(NULL, "ModelData NoneError", "Error", MB_OK);
		return false;
	}

	vertexCount = modelDate->vertCount;				//頂点データを取得
	vertexData = new VERTEX[vertexCount];

	for (int i = 0; i < vertexCount; i++) {
		vertexData[i].position.x = modelDate->pmdVertex[i].position.x;
		vertexData[i].position.y = modelDate->pmdVertex[i].position.y;
		vertexData[i].position.z = modelDate->pmdVertex[i].position.z;
		vertexData[i].nomal.x    = modelDate->pmdVertex[i].normal.x;
		vertexData[i].nomal.y    = modelDate->pmdVertex[i].normal.y;
		vertexData[i].nomal.z    = modelDate->pmdVertex[i].normal.z;
		vertexData[i].col.x		 = 0.0f;
		vertexData[i].col.y		 = 0.0f;
		vertexData[i].col.z		 = 0.0f;
		vertexData[i].col.w		 = 1.0f;
		vertexData[i].uv.x		 = modelDate->pmdVertex[i].uv.x;
		vertexData[i].uv.y		 = modelDate->pmdVertex[i].uv.y;
	}

	//インデックスデータの取得
	indexCount = modelDate->faceVertCount;
	indexData  = new unsigned short[indexCount];
	for (int i = 0; i < indexCount; i++) {
		indexData[i] = modelDate->faceVertIndex[i];
	}

	//カラーデータの取得
	colorCount  = modelDate->materialCount;

	for (int i = 0; i < colorCount; i++) {
		//コンスタントバッファにカラーデータを入れる
		cb.AmbientMaterial.x = modelDate->pmdMaterial[i].mirrorColor.x;	// 環境光の反射率	
		cb.AmbientMaterial.y = modelDate->pmdMaterial[i].mirrorColor.y;
		cb.AmbientMaterial.z = modelDate->pmdMaterial[i].mirrorColor.z;
		cb.AmbientMaterial.w = 1.0f;

		cb.DiffuseMaterial.x = modelDate->pmdMaterial[i].diffuseColor.x; // ディフューズ光の反射率
		cb.DiffuseMaterial.y = modelDate->pmdMaterial[i].diffuseColor.y;
		cb.DiffuseMaterial.z = modelDate->pmdMaterial[i].diffuseColor.z;
		cb.DiffuseMaterial.w = modelDate->pmdMaterial[i].diffuseColor.w;

		cb.SpecularMaterial.x = modelDate->pmdMaterial[i].specularColor.x;// スペキュラ光の反射率
		cb.SpecularMaterial.y = modelDate->pmdMaterial[i].specularColor.y;
		cb.SpecularMaterial.z = modelDate->pmdMaterial[i].specularColor.z;
		cb.SpecularMaterial.w = modelDate->pmdMaterial[i].speclarity;
	}
	//-----------------------------------------------------------------------------
	
	device		= CDirectXGraphics::GetInstance()->GetDXDevice();
	devcontext  = CDirectXGraphics::GetInstance()->GetImmediateContext();

	//頂点レイアウトの定義
	D3D11_INPUT_ELEMENT_DESC vlayout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,	    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }		//D3D11_APPEND_ALIGNED_ELEMENT -> 4*3
	};
	
	// 頂点バッファを生成
	sts = CreateVertexBuffer(
		device,						// デバイスオブジェクト
		sizeof(VERTEX),				// １頂点当たりバイト数
		vertexCount,				// 頂点数
		vertexData,					// 頂点データ格納メモリ先頭アドレス
		&vertexBuffer				// 頂点バッファ
	);
	if (!sts) {
		MessageBox(nullptr, "CreateVertexBuffer error", "error", MB_OK);
		return false;
	}

	//インデックスバッファの作成
	sts = CreateIndexBuffer(
		device,				// デバイスオブジェクト
		indexCount,			// インデックス数
		indexData,			// インデックスデータ格納メモリ先頭アドレス
		&indexBuffer		// インデックスバッファ
	);
	if (!sts) {
		MessageBox(nullptr, "CreateIndexBuffer error", "error", MB_OK);
		return false;
	}

	// コンスタントバッファ作成
	sts = CreateConstantBuffer(
		device,
		sizeof(CBDATA),
		&constantBuffer);
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(constant buffer Material) error", "Error", MB_OK);
		return false;
	}

	// 頂点シェーダーオブジェクトを生成、同時に頂点レイアウトも生成
	sts = CreateVertexShader(
		device,								// デバイスオブジェクト						
		vsfile,								// 頂点シェーダーソースファイル					
		"main",								// エントリ関数
		"vs_5_0",							// 頂点シェーダーモデル５．０
		vlayout,							// 頂点データレイアウト
		ARRAYSIZE(vlayout),					// 頂点データのエレメント数
		&vertexShader,						// 頂点シェーダーオブジェクト
		&vertexLayout);						// 頂点データレイアウトオブジェクト
	if (!sts) {
		MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
		return false;
	}

	// ピクセルシェーダーを生成
	sts = CreatePixelShader(			// ピクセルシェーダーオブジェクトを生成
		device,							// デバイスオブジェクト
		psfile,
		"main",
		"ps_5_0",
		&pixelShader);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	// シェーダーリソースビュー生成
	srv = new ID3D11ShaderResourceView*[colorCount];
	for (int i = 0; i < colorCount; i++) {
		std::string filename(modelDate->pmdMaterial[i].textureFileName);
		if(filename.length() != 0){
			filename = "texture/" + filename;
			bool sts = CreatetSRVfromFile(filename.c_str(),
				device,
				devcontext,
				&srv[i]
			);
			if (!sts) {
				MessageBox(NULL, "Not Texture Material", "Error", MB_OK);
				/*return false;*/
			}
		}
		else {
			bool sts = CreatetSRVfromWICFile("texture/white.bmp",		// WICファイルからシェーダーリソースビューを作成する
				device,
				devcontext,
				&srv[i]);
		}
	}
	return true;
}

void MMDBreaker::PMDModel::UnInit() {
	if (indexBuffer != nullptr) {			// インデックスバッファ解放
		indexBuffer->Release();
		indexBuffer = nullptr;
	}

	if (vertexBuffer != nullptr) {			// 頂点バッファ解放
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}

	if (constantBuffer != nullptr) {		// コンスタントバッファ解放
		constantBuffer->Release();
		constantBuffer = nullptr;
	}

	// 頂点シェーダー解放
	if (vertexShader) {
		vertexShader->Release();
		vertexShader = nullptr;
	}

	// ピクセルシェーダー解放
	if (pixelShader) {
		pixelShader->Release();
		pixelShader = nullptr;
	}

	// 頂点レイアウト解放
	if (vertexLayout) {
		vertexLayout->Release();
		vertexLayout = nullptr;
	}

	if (srv) {
		delete[] srv;
	}
}

void MMDBreaker::PMDModel::Draw() {
	//ここから先関数化できそう
	devcontext->IASetInputLayout(vertexLayout);				// 頂点フォーマットをセット
	devcontext->VSSetShader(vertexShader, nullptr, 0);		// 頂点シェーダーをセット
	devcontext->PSSetShader(pixelShader, nullptr, 0);		// ピクセルシェーダーをセット

	unsigned int stride = sizeof(VERTEX);					// ストライドをセット（１頂点当たりのバイト数）
	unsigned int offset = 0;								// オフセット値をセット

	devcontext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);			// 頂点バッファをデバイスコンテキストへセット
	devcontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);				// インデックスバッファをデバイスコンテキストへセット
	devcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		// トポロジーを設定

	int idx = 0;

	for (int i = 0; i < colorCount; i++) {

		devcontext->PSSetShaderResources(0, 1, &srv[i]);

		//サブセットの数だけループしながら定数バッファにデータをぶち込む
		cb.AmbientMaterial.x = modelDate->pmdMaterial[i].mirrorColor.x;	// 環境光の反射率	
		cb.AmbientMaterial.y = modelDate->pmdMaterial[i].mirrorColor.y;
		cb.AmbientMaterial.z = modelDate->pmdMaterial[i].mirrorColor.z;
		cb.AmbientMaterial.w = 1.0f;

		cb.DiffuseMaterial.x = modelDate->pmdMaterial[i].diffuseColor.x; // ディフューズ光の反射率
		cb.DiffuseMaterial.y = modelDate->pmdMaterial[i].diffuseColor.y;
		cb.DiffuseMaterial.z = modelDate->pmdMaterial[i].diffuseColor.z;
		cb.DiffuseMaterial.w = modelDate->pmdMaterial[i].diffuseColor.w;

		cb.SpecularMaterial.x = modelDate->pmdMaterial[i].specularColor.x;// スペキュラ光の反射率
		cb.SpecularMaterial.y = modelDate->pmdMaterial[i].specularColor.y;
		cb.SpecularMaterial.z = modelDate->pmdMaterial[i].specularColor.z;
		cb.SpecularMaterial.w = modelDate->pmdMaterial[i].speclarity;

		//定数バッファ更新
		devcontext->UpdateSubresource(
			constantBuffer,	// コンスタントバッファ
			0,				// コピー先サブリソース
			nullptr,		// サブリソースを定義するボックス　ＮＵＬＬの場合オフセットなし
			&cb,			// コピー元データ
			0,				// 1行のサイズ
			0);				// 1深度スライスのサイズ

		// 定数バッファ3を頂点シェーダーへセット
		devcontext->VSSetConstantBuffers(
			3,				 // スタートスロット
			1,				 // 個数
			&constantBuffer);// コンスタントバッファ

		// 定数バッファ3をピクセルシェーダーへセット
		devcontext->PSSetConstantBuffers(3, 1, &constantBuffer);

		devcontext->DrawIndexed(modelDate->pmdMaterial[i].faceVertCount, idx, 0);			// モデル描画
		idx += modelDate->pmdMaterial[i].faceVertCount;
	}
}
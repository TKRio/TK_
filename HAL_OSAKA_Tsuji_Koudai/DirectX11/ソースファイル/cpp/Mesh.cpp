//*****************************************************************************
//!	@file	Mesh.cpp
//!	@brief	メッシュを動的に作成する処理
//!	@note	テクスチャがあるメッシュを作れていません。
//!	@author
//*****************************************************************************

#include "Mesh.h"
#include "Camera.h"

//引数で送られてくるデータを基に、新しいメッシュを作る関数
bool MMDBreaker::Mesh::CreateMesh(const char *vx, const char *px, VERTEX* vert, std::vector<WORD> IL, PMDData *pmdModel) {
	bool sts;
	victimModel = pmdModel;
	if (victimModel == nullptr) {
		MessageBox(NULL, "victimModelData NoneError", "Error", MB_OK);
		return false;
	}

	DX11MtxIdentity(worldMtx);
	angle = { 0.0f,0.0f,0.0f };
	trans = { 0.0f,0.0f,0.0f };
	// デバイスを取得する
	device = CDirectXGraphics::GetInstance()->GetDXDevice();
	// デバイスコンテキストを取得する
	devicecontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	//デバッグ用
	//Vertex test[50];
	//for (int i = 0; i < vertexListSize; i++) {
	//	test[i] = vert[i];
	//}

	indexList = new WORD[indexListSize];
	for (int i = 0; i < indexListSize; i++) {
		indexList[i] = IL[i];
	}
	// 頂点データの定義
	D3D11_INPUT_ELEMENT_DESC vlayout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,	    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }		//D3D11_APPEND_ALIGNED_ELEMENT -> 4*3
	};

	// エレメント数
	unsigned int numElements = ARRAYSIZE(vlayout);

	// 頂点バッファを生成
	sts = CreateVertexBuffer(
		device,						// デバイスオブジェクト
		sizeof(VERTEX),				// １頂点当たりバイト数
		vertexListSize,				// 頂点数
		vert,						// 頂点データ格納メモリ先頭アドレス
		&vertexBuffer				// 頂点バッファ
	);
	if (!sts) {
		MessageBox(nullptr, "CreateVertexBuffer error", "error", MB_OK);
		return false;
	}

	//index
	sts = CreateIndexBuffer(
		device,
		indexListSize,
		indexList,
		&indexBuffer
	);
	if (!sts) {
		MessageBox(nullptr, "CreateIndexBuffer error", "error", MB_OK);
		return false;
	}

	//定数バッファ
	sts = CreateConstantBuffer(
		device,
		sizeof(CBDATA),
		&constantBuffer
	);
	if (!sts) {
		MessageBox(nullptr, "CreateConstantBuffer error", "error", MB_OK);
		return false;
	}

	// 頂点シェーダーオブジェクトを生成、同時に頂点レイアウトも生成
	sts = CreateVertexShader(
		device,								// デバイスオブジェクト						
		vx,									// 頂点シェーダーソースファイル					
		"main",								// エントリ関数
		"vs_5_0",							// 頂点シェーダーモデル５．０
		vlayout,							// 頂点データレイアウト
		numElements,						// 頂点データのエレメント数
		&vertexShader,					// 頂点シェーダーオブジェクト
		&vertexLayout);					// 頂点データレイアウトオブジェクト
	if (!sts) {
		MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
		return false;
	}

	// ピクセルシェーダーを生成
	sts = CreatePixelShader(		// ピクセルシェーダーオブジェクトを生成
		device,						// デバイスオブジェクト
		px,							// ピクセルシェーダーソースファイル
		"main",						// エントリ関数
		"ps_5_0",					// ピクセルシェーダーモデル５．０
		&pixelShader);				// ピクセルシェーダーオブジェクト
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	srv = new ID3D11ShaderResourceView*[colorCount];
	for (int i = 0; i < colorCount; i++) {
		std::string filename(victimModel->pmdMaterial[i].textureFileName);
		if (filename.length() != 0) {
			filename = "texture/" + filename;
			bool sts = CreatetSRVfromFile(filename.c_str(),
				device,
				devicecontext,
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
				devicecontext,
				&srv[i]);
		}
	}

	return true;
}

void MMDBreaker::Mesh::DrawMesh() {

	devicecontext->IASetInputLayout(vertexLayout);
	devicecontext->VSSetShader(vertexShader, nullptr, 0);		// 頂点シェーダーをセット
	devicecontext->PSSetShader(pixelShader, nullptr, 0);		// ピクセルシェーダーをセット

	unsigned int stride = sizeof(VERTEX);					// ストライドをセット（１頂点当たりのバイト数）
	unsigned  offset = 0;									// オフセット値をセット

	// 頂点バッファをデバイスコンテキストへセット
	devicecontext->IASetVertexBuffers(
		0,													// スタートスロット
		1,													// 頂点バッファ個数
		&vertexBuffer,										// 頂点バッファの先頭アドレス
		&stride,											// ストライド
		&offset);											// オフセット

	devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, worldMtx);

	int idx = 0;
	for (int i = 0; i < colorCount; i++) {

		devicecontext->PSSetShaderResources(0, 1, &srv[i]);

		cb.AmbientMaterial.x = victimModel->pmdMaterial[i].mirrorColor.x;	// 環境光の反射率	
		cb.AmbientMaterial.y = victimModel->pmdMaterial[i].mirrorColor.y;
		cb.AmbientMaterial.z = victimModel->pmdMaterial[i].mirrorColor.z;
		cb.AmbientMaterial.w = 1.0f;

		cb.DiffuseMaterial.x = victimModel->pmdMaterial[i].diffuseColor.x; // ディフューズ光の反射率
		cb.DiffuseMaterial.y = victimModel->pmdMaterial[i].diffuseColor.y;
		cb.DiffuseMaterial.z = victimModel->pmdMaterial[i].diffuseColor.z;
		cb.DiffuseMaterial.w = victimModel->pmdMaterial[i].diffuseColor.w;

		cb.SpecularMaterial.x = victimModel->pmdMaterial[i].specularColor.x;// スペキュラ光の反射率
		cb.SpecularMaterial.y = victimModel->pmdMaterial[i].specularColor.y;
		cb.SpecularMaterial.z = victimModel->pmdMaterial[i].specularColor.z;
		cb.SpecularMaterial.w = victimModel->pmdMaterial[i].speclarity;


		//定数バッファ更新
		devicecontext->UpdateSubresource(
			constantBuffer,	// コンスタントバッファ
			0,				// コピー先サブリソース
			nullptr,		// サブリソースを定義するボックス　ＮＵＬＬの場合オフセットなし
			&cb,			// コピー元データ
			0,				// 1行のサイズ
			0);				// 1深度スライスのサイズ

		// 定数バッファ3を頂点シェーダーへセット
		devicecontext->VSSetConstantBuffers(
			3,				 // スタートスロット
			1,				 // 個数
			&constantBuffer);// コンスタントバッファ

		// 定数バッファ3をピクセルシェーダーへセット
		devicecontext->PSSetConstantBuffers(3, 1, &constantBuffer);
		devicecontext->DrawIndexed(indexListSize, 0, 0);
		
		//devicecontext->DrawIndexed(victimModel->pmdMaterial[i].faceVertCount, idx, 0);			// モデル描画
		idx += victimModel->pmdMaterial[i].faceVertCount;
	}
}

void MMDBreaker::Mesh::UnInitMesh() {

	if (vertexLayout != nullptr) {
		vertexLayout->Release();
		vertexLayout = nullptr;
	}

	if (vertexShader != nullptr) {
		vertexShader->Release();
		vertexShader = nullptr;
	}

	if (pixelShader != nullptr) {
		pixelShader->Release();
		pixelShader = nullptr;
	}

	if (vertexBuffer != nullptr) {
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}

	if (constantBuffer != nullptr) {
		constantBuffer->Release();
		constantBuffer = nullptr;
	}
}
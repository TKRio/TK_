#include "2DQuadtex.h"

using namespace DirectX;
//******************************************************
//C2DBaseTex
//******************************************************
bool C2DBaseTex::Init(std::string filaname) {

	dev = CDirectXGraphics::GetInstance()->GetDXDevice();
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();
	CDirectXGraphics::GetInstance()->TurnOnAlphaBlending();

	// コンスタントバッファ作成
	bool sts = CreateConstantBuffer(
		dev,						// デバイス
		sizeof(CBVPDATA),			// サイズ
		&cBVP);						// コンスタントバッファビューポート
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(constant buffer viewport) error", "Error", MB_OK);
		return false;
	}

	// 頂点データの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	unsigned int numElements = ARRAYSIZE(layout);

	// 頂点シェーダーオブジェクトを生成、同時に頂点レイアウトも生成
	sts = CreateVertexShader(dev,
		"shader/2dquadtexvs.fx",
		"main",
		"vs_5_0",
		layout,
		numElements,
		&vertexShader,
		&vertexLayout);
	if (!sts) {
		MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
		return false;
	}

	// ピクセルシェーダーを生成
	sts = CreatePixelShader(			// ピクセルシェーダーオブジェクトを生成
		dev,								// デバイスオブジェクト
		"shader/2dquadtexps.fx",
		"main",
		"ps_5_0",
		&pixelShader);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	// シェーダーリソースビュー生成
	sts = CreatetSRVfromFile(filaname.c_str(), dev, devcontext, &srv);
	if (!sts) {
		MessageBox(nullptr, "CreatetSRVfromFile error", "error", MB_OK);
		return false;
	}

	return true;
}

void C2DBaseTex::UpdateCB() {

	int vph = CDirectXGraphics::GetInstance()->GetViewPortHeight();
	int vpw = CDirectXGraphics::GetInstance()->GetViewPortWidth();


	cBVPData5.viewPortHeight.x = static_cast<float>(vph);
	cBVPData5.viewPortWidth.x = static_cast<float>(vpw);

	devcontext->UpdateSubresource(cBVP, 0, nullptr, &cBVPData5, 0, 0);		// コンスタントバッファ更新
	devcontext->VSSetConstantBuffers(5, 1, &cBVP);							// コンスタントバッファをb5レジスタへセット
}

void C2DBaseTex::Uninit() {
	if (cBVP) {
		cBVP->Release();
		cBVP = nullptr;
	}
	// ピクセルシェーダー解放
	if (pixelShader) {
		pixelShader->Release();
		pixelShader = nullptr;
	}

	// 頂点シェーダー解放
	if (vertexShader) {
		vertexShader->Release();
		vertexShader = nullptr;
	}

	// 頂点レイアウト解放
	if (vertexLayout) {
		vertexLayout->Release();
		vertexLayout = nullptr;
	}

	// ＳＲＶ解放
	if (srv) {
		srv->Release();
		srv = nullptr;
	}
}

//******************************************************
//C2DQuadTex
//******************************************************

void C2DQuadTex::SetVertexData(float x, float y, float width, float height, XMFLOAT4 col) {
	// 頂点座標セット
	vertex2D[0].pos.x = x - width / 2.0f;
	vertex2D[0].pos.y = y - height / 2.0f;
	vertex2D[0].pos.z = 0.0f;
	vertex2D[0].color = col;
	vertex2D[0].tex.x = 0.0f;
	vertex2D[0].tex.y = 0.0f;

	vertex2D[1].pos.x = vertex2D[0].pos.x + width;
	vertex2D[1].pos.y = vertex2D[0].pos.y;
	vertex2D[1].pos.z = 0.0f;
	vertex2D[1].color = col;
	vertex2D[1].tex.x = 1.0f;
	vertex2D[1].tex.y = 0.0f;

	vertex2D[2].pos.x = vertex2D[0].pos.x;
	vertex2D[2].pos.y = vertex2D[0].pos.y + height;
	vertex2D[2].pos.z = 0.0f;
	vertex2D[2].color = col;
	vertex2D[2].tex.x = 0.0f;
	vertex2D[2].tex.y = 1.0f;

	vertex2D[3].pos.x = vertex2D[0].pos.x + width;
	vertex2D[3].pos.y = vertex2D[0].pos.y + height;
	vertex2D[3].pos.z = 0.0f;
	vertex2D[3].color = col;
	vertex2D[3].tex.x = 1.0f;
	vertex2D[3].tex.y = 1.0f;
}

bool C2DQuadTex::Init(float x, float y, float width, float height, XMFLOAT4 color, std::string filename) {

	bool sts;

	dev = CDirectXGraphics::GetInstance()->GetDXDevice();
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	// 基底クラスの初期処理
	C2DBaseTex::Init(filename);

	col = color;

	// 頂点座標セット	
	SetVertexData(x, y, width, height, color);

	// 頂点バッファ作成
	sts = CreateVertexBufferWrite(
		dev,
		sizeof(VERTEX2D),			// １頂点当たりバイト数
		4,						// 頂点数
		vertex2D,				// 頂点データ格納メモリ先頭アドレス
		&vertexBuffer);		// 頂点バッファ
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(vertex buffer) error", "Error", MB_OK);
		return false;
	}

	// ビューポート値を更新
	UpdateCB();

	return true;
}

void C2DQuadTex::Draw() {
	XMFLOAT3   trans;
	XMFLOAT4X4 mtx;

	trans.x = x;
	trans.y = y;
	trans.z = 0;

	DX11MtxTranslation(trans, mtx);
	// ワールド変換行列
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, mtx);

	// 頂点バッファをセットする
	unsigned int stride = sizeof(VERTEX2D);
	unsigned  offset = 0;
	devcontext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	devcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		// トポロジーをセット（旧プリミティブタイプ）

	// 頂点レイアウトセット
	devcontext->IASetInputLayout(vertexLayout);

	devcontext->VSSetShader(vertexShader, nullptr, 0);		// 頂点シェーダーをセット
	devcontext->PSSetShader(pixelShader, nullptr, 0);		// ピクセルシェーダーをセット

	devcontext->PSSetShaderResources(0, 1, &srv);

	devcontext->Draw(4, 0);									// 描画するインデックス数(4)

}

void C2DQuadTex::Uninit() {
	C2DBaseTex::Uninit();
	if (vertexBuffer) {
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}
}

void C2DQuadTex::ChangePosSize(float x, float y, float width, float height) {
	D3D11_MAPPED_SUBRESOURCE pData;

	HRESULT hr = devcontext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	if (SUCCEEDED(hr)) {
		SetVertexData(x, y, width, height, col);
		memcpy_s(pData.pData, pData.RowPitch, (void*)(vertex2D), sizeof(VERTEX2D) * 4);
		devcontext->Unmap(vertexBuffer, 0);
	}
}

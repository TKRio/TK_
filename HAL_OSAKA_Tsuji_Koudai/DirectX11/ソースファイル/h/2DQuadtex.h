//*****************************************************************************
//!	@file	2DQuadtex.h
//!	@brief	2D画像の描画
//!	@note
//!	@author
//*****************************************************************************

#pragma once
#include	<d3d11.h>
#include	<DirectXMath.h>
#include	"CDirectxGraphics.h"
#include	"DX11Settransform.h"
#include	"Shader.h"
#include	"dx11mathutil.h"

//******************************************************************************
//class
//******************************************************************************

//C2DBaseTex
class C2DBaseTex {
protected:
	typedef struct _ConstantBufferViewPort_
	{
		DirectX::XMFLOAT4	viewPortWidth;			// ビューポート幅
		DirectX::XMFLOAT4	viewPortHeight;			// ビューポート高さ
	}CBVPDATA;

	typedef struct _Vertex2D_ {
		DirectX::XMFLOAT3	pos;			// 座標
		DirectX::XMFLOAT4	color;			// 頂点カラー
		DirectX::XMFLOAT2	tex;			// テクスチャ座標
	}VERTEX2D;

	ID3D11Buffer*			vertexBuffer = nullptr;	// 頂点バッファ
	ID3D11VertexShader*		vertexShader = nullptr;	// 頂点シェーダー
	ID3D11PixelShader*		pixelShader  = nullptr;	// ピクセルシェーダー
	ID3D11InputLayout*		vertexLayout = nullptr;	// 頂点レイアウト

	ID3D11Device*			dev = nullptr;			// DIRECT3DDEVICE11デバイス
	ID3D11DeviceContext*	devcontext = nullptr;		// DIRECT3DDEVICE11デバイスコンテキスト

	DirectX::XMFLOAT4		col;
	ID3D11Buffer*			cBVP		= nullptr;	// コンスタントバッファ
	CBVPDATA				cBVPData5;				// b5

	ID3D11ShaderResourceView* srv = nullptr;		// shader resource view

public:
	bool Init(std::string filaname);
	void UpdateCB();
	void Uninit();
};

//C2DQuadTex
class C2DQuadTex :public C2DBaseTex{
private:
	float	x = 0;
	float	y = 0;

	VERTEX2D				vertex2D[4];		// 頂点座標＊４
	void SetVertexData(float x, float y, float width, float height, DirectX::XMFLOAT4 col); // ＸＹ平面
public:
	bool Init(float x, float y, float width, float height, DirectX::XMFLOAT4 color, std::string filename);
	void Draw();
	void Uninit();

	void ChangePosSize(float x, float y, float width, float height);
};

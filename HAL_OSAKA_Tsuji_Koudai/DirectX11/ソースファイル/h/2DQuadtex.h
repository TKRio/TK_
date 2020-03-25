//*****************************************************************************
//!	@file	2DQuadtex.h
//!	@brief	2D�摜�̕`��
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
		DirectX::XMFLOAT4	viewPortWidth;			// �r���[�|�[�g��
		DirectX::XMFLOAT4	viewPortHeight;			// �r���[�|�[�g����
	}CBVPDATA;

	typedef struct _Vertex2D_ {
		DirectX::XMFLOAT3	pos;			// ���W
		DirectX::XMFLOAT4	color;			// ���_�J���[
		DirectX::XMFLOAT2	tex;			// �e�N�X�`�����W
	}VERTEX2D;

	ID3D11Buffer*			vertexBuffer = nullptr;	// ���_�o�b�t�@
	ID3D11VertexShader*		vertexShader = nullptr;	// ���_�V�F�[�_�[
	ID3D11PixelShader*		pixelShader  = nullptr;	// �s�N�Z���V�F�[�_�[
	ID3D11InputLayout*		vertexLayout = nullptr;	// ���_���C�A�E�g

	ID3D11Device*			dev = nullptr;			// DIRECT3DDEVICE11�f�o�C�X
	ID3D11DeviceContext*	devcontext = nullptr;		// DIRECT3DDEVICE11�f�o�C�X�R���e�L�X�g

	DirectX::XMFLOAT4		col;
	ID3D11Buffer*			cBVP		= nullptr;	// �R���X�^���g�o�b�t�@
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

	VERTEX2D				vertex2D[4];		// ���_���W���S
	void SetVertexData(float x, float y, float width, float height, DirectX::XMFLOAT4 col); // �w�x����
public:
	bool Init(float x, float y, float width, float height, DirectX::XMFLOAT4 color, std::string filename);
	void Draw();
	void Uninit();

	void ChangePosSize(float x, float y, float width, float height);
};

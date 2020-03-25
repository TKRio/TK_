//*****************************************************************************
//!	@file	game.cpp
//!	@brief	
//!	@note	�Q�[������
//!	@author	
//*****************************************************************************
#include "game.h"
#include "Scene.h"
using namespace DirectX;

//*****************************************************************************
// �O���|�o���錾
//*****************************************************************************
Scene scene;

//==============================================================================
//!	@fn		GameInit
//!	@brief	�Q�[����������
//!	@param	�C���X�^���X�l
//!	@param	�E�C���h�E�n���h���l
//!	@param	�E�C���h�E�T�C�Y���i�␳�ς݂̒l�j
//!	@param	�E�C���h�E�T�C�Y�����i�␳�ς݂̒l�j
//!	@param�@	�t���X�N���[���t���O�@true ; �t���X�N���[���@false : �E�C���h�E
//!	@retval	true �����@false ���s
//==============================================================================
bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height, bool fullscreen)
{
	bool		sts;
	// DX11��������
	sts = DX11Init(hwnd, width, height, fullscreen);
	if (!sts) {
		MessageBox(hwnd, "DX11 init error", "error", MB_OK);
		return false;
	}

	//�v���W�F�N�V�����ϊ��s�񏉊���(�J����)
	XMFLOAT3 eye = {0.0f,10.0f,-18.0f};				// ���_
	XMFLOAT3 lookat = { 0.0f,10.0f,0.0f };			// �����_
	XMFLOAT3 up = {0.0f,1.0f,0.0f};					// ������x�N�g��

	Camera::GetInstance()->Init(1.0f, 10000.0f, XM_PIDIV2, SCREEN_X, SCREEN_Y, eye, lookat, up);

	//�萔�o�b�t�@�̏�����
	DX11SetTransform::GetInstance()->Init();

	// DIRECTINPUT������
	CDirectInput::GetInstance().Init(hinst, hwnd, width, height);

	// ���C�g������
	DX11LightInit(DirectX::XMFLOAT4(0,0,-1,0));		// ���s�����̕������Z�b�g

	scene.Init(hwnd);

	return	true;
}

//==============================================================================
//!	@fn		GameInput
//!	@brief	�Q�[�����͌��m����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameInput(){
	CDirectInput::GetInstance().GetKeyBuffer();			// �L�[���͎擾
	}

//==============================================================================
//!	@fn		GameUpdate
//!	@brief	�Q�[���X�V����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameUpdate(){
	scene.Updata();
}

//==============================================================================
//!	@fn		GameRender
//!	@brief	�Q�[���X�V����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameRender(){
	// �^�[�Q�b�g�o�b�t�@�N���A	
	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; //red,green,blue,alpha
	
	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* devcontext;
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* rtv;
	rtv = CDirectXGraphics::GetInstance()->GetRenderTargetView();

	// �^�[�Q�b�g�o�b�t�@�N���A
	devcontext->ClearRenderTargetView(
		rtv,			// �����_�[�^�[�Q�b�g�r���[
		ClearColor);	// �N���A����J���[�l

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* dstv;
	dstv = CDirectXGraphics::GetInstance()->GetDepthStencilView();

	// Z�o�b�t�@�A�X�e���V���o�b�t�@�N���A
	devcontext->ClearDepthStencilView(
		dstv,			// �f�v�X�X�e���V���r���[
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,			// �y�o�b�t�@��1.0�ŃN���A����(0.0f�`1.0f)
		0);				// �X�e���V���o�b�t�@��0�ŃN���A����


	XMFLOAT4X4 mtx;		// �s��

	// �r���[�ϊ��s��Z�b�g
	mtx = Camera::GetInstance()->GetCameraMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::VIEW, mtx);

	// �v���W�F�N�V�����ϊ��s��Z�b�g
	mtx = Camera::GetInstance()->GetProjectionMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::PROJECTION, mtx);

	scene.Draw();

	// �`��㏈��
	IDXGISwapChain* swapchain;
	swapchain = CDirectXGraphics::GetInstance()->GetSwapChain();
	swapchain->Present(
		0,		// �t���[���̕\���𐂒������̋󔒂Ɠ�����������@���w�肷�鐮���B
		0);		// �X���b�v�`�F�[���\���I�v�V�������܂ސ����l�B 
				// �����̃I�v�V������DXGI_PRESENT�萔�Œ�`����Ă��܂��B
	
}

//==============================================================================
//!	@fn		GameMain
//!	@brief	�Q�[�����[�v����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameMain()
{
	GameInput();					// ����
	GameUpdate();					// �X�V
	GameRender();					// �`��
}

//==============================================================================
//!	@fn		GameExit
//!	@brief	�Q�[���I������
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameExit()
{
	scene.Uninit();
	DX11Uninit();
}

//******************************************************************************
//	End of file.
//******************************************************************************

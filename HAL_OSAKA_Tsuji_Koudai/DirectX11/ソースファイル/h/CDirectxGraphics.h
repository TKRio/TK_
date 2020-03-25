#pragma once
#include	<d3d11.h>

class CDirectXGraphics{
private:
	CDirectXGraphics() {}									// �R���X�g���N�^
	ID3D11Device*				m_lpDevice;					// DIRECT3DDEVICE11�f�o�C�X
	ID3D11DeviceContext*		m_lpImmediateContext;		// DIRECT3DDEVICE11�f�o�C�X�R���e�L�X�g
	IDXGISwapChain*				m_lpSwapChain;				// �X���b�v�`�F�C��

	ID3D11RenderTargetView*		m_lpRenderTargetView;		// �����_�[�^�[�Q�b�g�r���[
	ID3D11Texture2D*			m_depthStencilBuffer;		// �y�o�b�t�@�A�X�e���V���o�b�t�@
	ID3D11DepthStencilState*	m_depthStencilState;		// �y�o�b�t�@�A�X�e���V���X�e�[�g
	ID3D11DepthStencilView*		m_depthStencilView;			// �y�o�b�t�@�A�X�e���V���r���[
	ID3D11RasterizerState*		m_rasterState;				// ���X�^�[�X�e�[�^�X

	ID3D11SamplerState*			m_samplerstate = nullptr;	// �T���v���[�X�e�[�g

	D3D_DRIVER_TYPE				m_DriverType;				// �h���C�o�^�C�v
	D3D_FEATURE_LEVEL			m_FeatureLevel;				// �@�\���x��

	ID3D11BlendState*			m_alphaEnableBlendingState = nullptr;
	ID3D11BlendState*			m_alphaDisableBlendingState = nullptr;

	int							m_Width;					// �o�b�N�o�b�t�@�w�T�C�Y
	int							m_Height;					// �o�b�N�o�b�t�@�x�T�C�Y
public:
	
	CDirectXGraphics(const CDirectXGraphics&) = delete;
	CDirectXGraphics& operator=(const CDirectXGraphics&) = delete;
	CDirectXGraphics(CDirectXGraphics&&) = delete;
	CDirectXGraphics& operator=(CDirectXGraphics&&) = delete;

	static CDirectXGraphics* GetInstance() {
		static CDirectXGraphics instance;
		return &instance;
	}
	// ��������
	bool Init(HWND hWnd, unsigned int Width, unsigned int Height, bool fullscreen = false);
	
	// �I������
	void	Exit();														

	// �f�o�C�XGET
	ID3D11Device*	GetDXDevice() const {							
		return m_lpDevice;
	}

	// �C�~�f�B�G�C�g�f�o�C�XGET
	ID3D11DeviceContext* GetImmediateContext() const {
		return m_lpImmediateContext;
	}

	// SWAP�`�F�C��GET
	IDXGISwapChain* GetSwapChain()const {
		return m_lpSwapChain;
	}

	// �����_�����O�^�[�Q�b�gGET
	ID3D11RenderTargetView* GetRenderTargetView() const{
		return m_lpRenderTargetView;
	}

	// depthstencil view
	ID3D11DepthStencilView* GetDepthStencilView() const{
		return m_depthStencilView;
	}
	// �r���[�|�[�g�̍������擾����
	int GetViewPortHeight() const {
		return m_Height;
	}

	// �r���[�|�[�g�̕����擾����
	int GetViewPortWidth() const {
		return m_Width;
	}

	// �A���t�@�u�����h�L����
	void TurnOnAlphaBlending();

	// �A���t�@�u�����h������
	void TurnOffAlphaBlending();

	// Z�o�b�t�@������
	void TurnOffZbuffer() {
		ID3D11RenderTargetView*  rtvtable[1];

		rtvtable[0] = m_lpRenderTargetView;

		m_lpImmediateContext->OMSetRenderTargets(
			1,										// �^�[�Q�b�g
			rtvtable,								// �r���[�e�[�u��
			nullptr									// �[�x�o�b�t�@�Ȃ�
		);
	}

	// Z�o�b�t�@�L����
	void TurnOnZBuffer() {
		ID3D11RenderTargetView*  rtvtable[1];

		rtvtable[0] = m_lpRenderTargetView;

		m_lpImmediateContext->OMSetRenderTargets(
			1,										// �^�[�Q�b�g
			rtvtable,								// �r���[�e�[�u��
			m_depthStencilView						// �[�x�o�b�t�@�Ȃ�
		);
	}
};

#include "2DQuadtex.h"

using namespace DirectX;
//******************************************************
//C2DBaseTex
//******************************************************
bool C2DBaseTex::Init(std::string filaname) {

	dev = CDirectXGraphics::GetInstance()->GetDXDevice();
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();
	CDirectXGraphics::GetInstance()->TurnOnAlphaBlending();

	// �R���X�^���g�o�b�t�@�쐬
	bool sts = CreateConstantBuffer(
		dev,						// �f�o�C�X
		sizeof(CBVPDATA),			// �T�C�Y
		&cBVP);						// �R���X�^���g�o�b�t�@�r���[�|�[�g
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(constant buffer viewport) error", "Error", MB_OK);
		return false;
	}

	// ���_�f�[�^�̒�`
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	unsigned int numElements = ARRAYSIZE(layout);

	// ���_�V�F�[�_�[�I�u�W�F�N�g�𐶐��A�����ɒ��_���C�A�E�g������
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

	// �s�N�Z���V�F�[�_�[�𐶐�
	sts = CreatePixelShader(			// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐�
		dev,								// �f�o�C�X�I�u�W�F�N�g
		"shader/2dquadtexps.fx",
		"main",
		"ps_5_0",
		&pixelShader);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	// �V�F�[�_�[���\�[�X�r���[����
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

	devcontext->UpdateSubresource(cBVP, 0, nullptr, &cBVPData5, 0, 0);		// �R���X�^���g�o�b�t�@�X�V
	devcontext->VSSetConstantBuffers(5, 1, &cBVP);							// �R���X�^���g�o�b�t�@��b5���W�X�^�փZ�b�g
}

void C2DBaseTex::Uninit() {
	if (cBVP) {
		cBVP->Release();
		cBVP = nullptr;
	}
	// �s�N�Z���V�F�[�_�[���
	if (pixelShader) {
		pixelShader->Release();
		pixelShader = nullptr;
	}

	// ���_�V�F�[�_�[���
	if (vertexShader) {
		vertexShader->Release();
		vertexShader = nullptr;
	}

	// ���_���C�A�E�g���
	if (vertexLayout) {
		vertexLayout->Release();
		vertexLayout = nullptr;
	}

	// �r�q�u���
	if (srv) {
		srv->Release();
		srv = nullptr;
	}
}

//******************************************************
//C2DQuadTex
//******************************************************

void C2DQuadTex::SetVertexData(float x, float y, float width, float height, XMFLOAT4 col) {
	// ���_���W�Z�b�g
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

	// ���N���X�̏�������
	C2DBaseTex::Init(filename);

	col = color;

	// ���_���W�Z�b�g	
	SetVertexData(x, y, width, height, color);

	// ���_�o�b�t�@�쐬
	sts = CreateVertexBufferWrite(
		dev,
		sizeof(VERTEX2D),			// �P���_������o�C�g��
		4,						// ���_��
		vertex2D,				// ���_�f�[�^�i�[�������擪�A�h���X
		&vertexBuffer);		// ���_�o�b�t�@
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(vertex buffer) error", "Error", MB_OK);
		return false;
	}

	// �r���[�|�[�g�l���X�V
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
	// ���[���h�ϊ��s��
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, mtx);

	// ���_�o�b�t�@���Z�b�g����
	unsigned int stride = sizeof(VERTEX2D);
	unsigned  offset = 0;
	devcontext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	devcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		// �g�|���W�[���Z�b�g�i���v���~�e�B�u�^�C�v�j

	// ���_���C�A�E�g�Z�b�g
	devcontext->IASetInputLayout(vertexLayout);

	devcontext->VSSetShader(vertexShader, nullptr, 0);		// ���_�V�F�[�_�[���Z�b�g
	devcontext->PSSetShader(pixelShader, nullptr, 0);		// �s�N�Z���V�F�[�_�[���Z�b�g

	devcontext->PSSetShaderResources(0, 1, &srv);

	devcontext->Draw(4, 0);									// �`�悷��C���f�b�N�X��(4)

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

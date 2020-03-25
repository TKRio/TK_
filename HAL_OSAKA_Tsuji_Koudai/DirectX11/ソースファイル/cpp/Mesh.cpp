//*****************************************************************************
//!	@file	Mesh.cpp
//!	@brief	���b�V���𓮓I�ɍ쐬���鏈��
//!	@note	�e�N�X�`�������郁�b�V�������Ă��܂���B
//!	@author
//*****************************************************************************

#include "Mesh.h"
#include "Camera.h"

//�����ő����Ă���f�[�^����ɁA�V�������b�V�������֐�
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
	// �f�o�C�X���擾����
	device = CDirectXGraphics::GetInstance()->GetDXDevice();
	// �f�o�C�X�R���e�L�X�g���擾����
	devicecontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	//�f�o�b�O�p
	//Vertex test[50];
	//for (int i = 0; i < vertexListSize; i++) {
	//	test[i] = vert[i];
	//}

	indexList = new WORD[indexListSize];
	for (int i = 0; i < indexListSize; i++) {
		indexList[i] = IL[i];
	}
	// ���_�f�[�^�̒�`
	D3D11_INPUT_ELEMENT_DESC vlayout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,	    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }		//D3D11_APPEND_ALIGNED_ELEMENT -> 4*3
	};

	// �G�������g��
	unsigned int numElements = ARRAYSIZE(vlayout);

	// ���_�o�b�t�@�𐶐�
	sts = CreateVertexBuffer(
		device,						// �f�o�C�X�I�u�W�F�N�g
		sizeof(VERTEX),				// �P���_������o�C�g��
		vertexListSize,				// ���_��
		vert,						// ���_�f�[�^�i�[�������擪�A�h���X
		&vertexBuffer				// ���_�o�b�t�@
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

	//�萔�o�b�t�@
	sts = CreateConstantBuffer(
		device,
		sizeof(CBDATA),
		&constantBuffer
	);
	if (!sts) {
		MessageBox(nullptr, "CreateConstantBuffer error", "error", MB_OK);
		return false;
	}

	// ���_�V�F�[�_�[�I�u�W�F�N�g�𐶐��A�����ɒ��_���C�A�E�g������
	sts = CreateVertexShader(
		device,								// �f�o�C�X�I�u�W�F�N�g						
		vx,									// ���_�V�F�[�_�[�\�[�X�t�@�C��					
		"main",								// �G���g���֐�
		"vs_5_0",							// ���_�V�F�[�_�[���f���T�D�O
		vlayout,							// ���_�f�[�^���C�A�E�g
		numElements,						// ���_�f�[�^�̃G�������g��
		&vertexShader,					// ���_�V�F�[�_�[�I�u�W�F�N�g
		&vertexLayout);					// ���_�f�[�^���C�A�E�g�I�u�W�F�N�g
	if (!sts) {
		MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
		return false;
	}

	// �s�N�Z���V�F�[�_�[�𐶐�
	sts = CreatePixelShader(		// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐�
		device,						// �f�o�C�X�I�u�W�F�N�g
		px,							// �s�N�Z���V�F�[�_�[�\�[�X�t�@�C��
		"main",						// �G���g���֐�
		"ps_5_0",					// �s�N�Z���V�F�[�_�[���f���T�D�O
		&pixelShader);				// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g
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
			bool sts = CreatetSRVfromWICFile("texture/white.bmp",		// WIC�t�@�C������V�F�[�_�[���\�[�X�r���[���쐬����
				device,
				devicecontext,
				&srv[i]);
		}
	}

	return true;
}

void MMDBreaker::Mesh::DrawMesh() {

	devicecontext->IASetInputLayout(vertexLayout);
	devicecontext->VSSetShader(vertexShader, nullptr, 0);		// ���_�V�F�[�_�[���Z�b�g
	devicecontext->PSSetShader(pixelShader, nullptr, 0);		// �s�N�Z���V�F�[�_�[���Z�b�g

	unsigned int stride = sizeof(VERTEX);					// �X�g���C�h���Z�b�g�i�P���_������̃o�C�g���j
	unsigned  offset = 0;									// �I�t�Z�b�g�l���Z�b�g

	// ���_�o�b�t�@���f�o�C�X�R���e�L�X�g�փZ�b�g
	devicecontext->IASetVertexBuffers(
		0,													// �X�^�[�g�X���b�g
		1,													// ���_�o�b�t�@��
		&vertexBuffer,										// ���_�o�b�t�@�̐擪�A�h���X
		&stride,											// �X�g���C�h
		&offset);											// �I�t�Z�b�g

	devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, worldMtx);

	int idx = 0;
	for (int i = 0; i < colorCount; i++) {

		devicecontext->PSSetShaderResources(0, 1, &srv[i]);

		cb.AmbientMaterial.x = victimModel->pmdMaterial[i].mirrorColor.x;	// �����̔��˗�	
		cb.AmbientMaterial.y = victimModel->pmdMaterial[i].mirrorColor.y;
		cb.AmbientMaterial.z = victimModel->pmdMaterial[i].mirrorColor.z;
		cb.AmbientMaterial.w = 1.0f;

		cb.DiffuseMaterial.x = victimModel->pmdMaterial[i].diffuseColor.x; // �f�B�t���[�Y���̔��˗�
		cb.DiffuseMaterial.y = victimModel->pmdMaterial[i].diffuseColor.y;
		cb.DiffuseMaterial.z = victimModel->pmdMaterial[i].diffuseColor.z;
		cb.DiffuseMaterial.w = victimModel->pmdMaterial[i].diffuseColor.w;

		cb.SpecularMaterial.x = victimModel->pmdMaterial[i].specularColor.x;// �X�y�L�������̔��˗�
		cb.SpecularMaterial.y = victimModel->pmdMaterial[i].specularColor.y;
		cb.SpecularMaterial.z = victimModel->pmdMaterial[i].specularColor.z;
		cb.SpecularMaterial.w = victimModel->pmdMaterial[i].speclarity;


		//�萔�o�b�t�@�X�V
		devicecontext->UpdateSubresource(
			constantBuffer,	// �R���X�^���g�o�b�t�@
			0,				// �R�s�[��T�u���\�[�X
			nullptr,		// �T�u���\�[�X���`����{�b�N�X�@�m�t�k�k�̏ꍇ�I�t�Z�b�g�Ȃ�
			&cb,			// �R�s�[���f�[�^
			0,				// 1�s�̃T�C�Y
			0);				// 1�[�x�X���C�X�̃T�C�Y

		// �萔�o�b�t�@3�𒸓_�V�F�[�_�[�փZ�b�g
		devicecontext->VSSetConstantBuffers(
			3,				 // �X�^�[�g�X���b�g
			1,				 // ��
			&constantBuffer);// �R���X�^���g�o�b�t�@

		// �萔�o�b�t�@3���s�N�Z���V�F�[�_�[�փZ�b�g
		devicecontext->PSSetConstantBuffers(3, 1, &constantBuffer);
		devicecontext->DrawIndexed(indexListSize, 0, 0);
		
		//devicecontext->DrawIndexed(victimModel->pmdMaterial[i].faceVertCount, idx, 0);			// ���f���`��
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
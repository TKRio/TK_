//*****************************************************************************
//!	@file	Model.cpp
//!	@brief	PMD&PMX��ǂݍ���Ń��f����`�悷��
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
	modelDate = new PMDData(fileName);		//���f���f�[�^�̎擾	
	if (modelDate == nullptr) {
		MessageBox(NULL, "ModelData NoneError", "Error", MB_OK);
		return false;
	}

	vertexCount = modelDate->vertCount;				//���_�f�[�^���擾
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

	//�C���f�b�N�X�f�[�^�̎擾
	indexCount = modelDate->faceVertCount;
	indexData  = new unsigned short[indexCount];
	for (int i = 0; i < indexCount; i++) {
		indexData[i] = modelDate->faceVertIndex[i];
	}

	//�J���[�f�[�^�̎擾
	colorCount  = modelDate->materialCount;

	for (int i = 0; i < colorCount; i++) {
		//�R���X�^���g�o�b�t�@�ɃJ���[�f�[�^������
		cb.AmbientMaterial.x = modelDate->pmdMaterial[i].mirrorColor.x;	// �����̔��˗�	
		cb.AmbientMaterial.y = modelDate->pmdMaterial[i].mirrorColor.y;
		cb.AmbientMaterial.z = modelDate->pmdMaterial[i].mirrorColor.z;
		cb.AmbientMaterial.w = 1.0f;

		cb.DiffuseMaterial.x = modelDate->pmdMaterial[i].diffuseColor.x; // �f�B�t���[�Y���̔��˗�
		cb.DiffuseMaterial.y = modelDate->pmdMaterial[i].diffuseColor.y;
		cb.DiffuseMaterial.z = modelDate->pmdMaterial[i].diffuseColor.z;
		cb.DiffuseMaterial.w = modelDate->pmdMaterial[i].diffuseColor.w;

		cb.SpecularMaterial.x = modelDate->pmdMaterial[i].specularColor.x;// �X�y�L�������̔��˗�
		cb.SpecularMaterial.y = modelDate->pmdMaterial[i].specularColor.y;
		cb.SpecularMaterial.z = modelDate->pmdMaterial[i].specularColor.z;
		cb.SpecularMaterial.w = modelDate->pmdMaterial[i].speclarity;
	}
	//-----------------------------------------------------------------------------
	
	device		= CDirectXGraphics::GetInstance()->GetDXDevice();
	devcontext  = CDirectXGraphics::GetInstance()->GetImmediateContext();

	//���_���C�A�E�g�̒�`
	D3D11_INPUT_ELEMENT_DESC vlayout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,	    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }		//D3D11_APPEND_ALIGNED_ELEMENT -> 4*3
	};
	
	// ���_�o�b�t�@�𐶐�
	sts = CreateVertexBuffer(
		device,						// �f�o�C�X�I�u�W�F�N�g
		sizeof(VERTEX),				// �P���_������o�C�g��
		vertexCount,				// ���_��
		vertexData,					// ���_�f�[�^�i�[�������擪�A�h���X
		&vertexBuffer				// ���_�o�b�t�@
	);
	if (!sts) {
		MessageBox(nullptr, "CreateVertexBuffer error", "error", MB_OK);
		return false;
	}

	//�C���f�b�N�X�o�b�t�@�̍쐬
	sts = CreateIndexBuffer(
		device,				// �f�o�C�X�I�u�W�F�N�g
		indexCount,			// �C���f�b�N�X��
		indexData,			// �C���f�b�N�X�f�[�^�i�[�������擪�A�h���X
		&indexBuffer		// �C���f�b�N�X�o�b�t�@
	);
	if (!sts) {
		MessageBox(nullptr, "CreateIndexBuffer error", "error", MB_OK);
		return false;
	}

	// �R���X�^���g�o�b�t�@�쐬
	sts = CreateConstantBuffer(
		device,
		sizeof(CBDATA),
		&constantBuffer);
	if (!sts) {
		MessageBox(NULL, "CreateBuffer(constant buffer Material) error", "Error", MB_OK);
		return false;
	}

	// ���_�V�F�[�_�[�I�u�W�F�N�g�𐶐��A�����ɒ��_���C�A�E�g������
	sts = CreateVertexShader(
		device,								// �f�o�C�X�I�u�W�F�N�g						
		vsfile,								// ���_�V�F�[�_�[�\�[�X�t�@�C��					
		"main",								// �G���g���֐�
		"vs_5_0",							// ���_�V�F�[�_�[���f���T�D�O
		vlayout,							// ���_�f�[�^���C�A�E�g
		ARRAYSIZE(vlayout),					// ���_�f�[�^�̃G�������g��
		&vertexShader,						// ���_�V�F�[�_�[�I�u�W�F�N�g
		&vertexLayout);						// ���_�f�[�^���C�A�E�g�I�u�W�F�N�g
	if (!sts) {
		MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
		return false;
	}

	// �s�N�Z���V�F�[�_�[�𐶐�
	sts = CreatePixelShader(			// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�𐶐�
		device,							// �f�o�C�X�I�u�W�F�N�g
		psfile,
		"main",
		"ps_5_0",
		&pixelShader);
	if (!sts) {
		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
		return false;
	}

	// �V�F�[�_�[���\�[�X�r���[����
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
			bool sts = CreatetSRVfromWICFile("texture/white.bmp",		// WIC�t�@�C������V�F�[�_�[���\�[�X�r���[���쐬����
				device,
				devcontext,
				&srv[i]);
		}
	}
	return true;
}

void MMDBreaker::PMDModel::UnInit() {
	if (indexBuffer != nullptr) {			// �C���f�b�N�X�o�b�t�@���
		indexBuffer->Release();
		indexBuffer = nullptr;
	}

	if (vertexBuffer != nullptr) {			// ���_�o�b�t�@���
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}

	if (constantBuffer != nullptr) {		// �R���X�^���g�o�b�t�@���
		constantBuffer->Release();
		constantBuffer = nullptr;
	}

	// ���_�V�F�[�_�[���
	if (vertexShader) {
		vertexShader->Release();
		vertexShader = nullptr;
	}

	// �s�N�Z���V�F�[�_�[���
	if (pixelShader) {
		pixelShader->Release();
		pixelShader = nullptr;
	}

	// ���_���C�A�E�g���
	if (vertexLayout) {
		vertexLayout->Release();
		vertexLayout = nullptr;
	}

	if (srv) {
		delete[] srv;
	}
}

void MMDBreaker::PMDModel::Draw() {
	//���������֐����ł�����
	devcontext->IASetInputLayout(vertexLayout);				// ���_�t�H�[�}�b�g���Z�b�g
	devcontext->VSSetShader(vertexShader, nullptr, 0);		// ���_�V�F�[�_�[���Z�b�g
	devcontext->PSSetShader(pixelShader, nullptr, 0);		// �s�N�Z���V�F�[�_�[���Z�b�g

	unsigned int stride = sizeof(VERTEX);					// �X�g���C�h���Z�b�g�i�P���_������̃o�C�g���j
	unsigned int offset = 0;								// �I�t�Z�b�g�l���Z�b�g

	devcontext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);			// ���_�o�b�t�@���f�o�C�X�R���e�L�X�g�փZ�b�g
	devcontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);				// �C���f�b�N�X�o�b�t�@���f�o�C�X�R���e�L�X�g�փZ�b�g
	devcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		// �g�|���W�[��ݒ�

	int idx = 0;

	for (int i = 0; i < colorCount; i++) {

		devcontext->PSSetShaderResources(0, 1, &srv[i]);

		//�T�u�Z�b�g�̐��������[�v���Ȃ���萔�o�b�t�@�Ƀf�[�^���Ԃ�����
		cb.AmbientMaterial.x = modelDate->pmdMaterial[i].mirrorColor.x;	// �����̔��˗�	
		cb.AmbientMaterial.y = modelDate->pmdMaterial[i].mirrorColor.y;
		cb.AmbientMaterial.z = modelDate->pmdMaterial[i].mirrorColor.z;
		cb.AmbientMaterial.w = 1.0f;

		cb.DiffuseMaterial.x = modelDate->pmdMaterial[i].diffuseColor.x; // �f�B�t���[�Y���̔��˗�
		cb.DiffuseMaterial.y = modelDate->pmdMaterial[i].diffuseColor.y;
		cb.DiffuseMaterial.z = modelDate->pmdMaterial[i].diffuseColor.z;
		cb.DiffuseMaterial.w = modelDate->pmdMaterial[i].diffuseColor.w;

		cb.SpecularMaterial.x = modelDate->pmdMaterial[i].specularColor.x;// �X�y�L�������̔��˗�
		cb.SpecularMaterial.y = modelDate->pmdMaterial[i].specularColor.y;
		cb.SpecularMaterial.z = modelDate->pmdMaterial[i].specularColor.z;
		cb.SpecularMaterial.w = modelDate->pmdMaterial[i].speclarity;

		//�萔�o�b�t�@�X�V
		devcontext->UpdateSubresource(
			constantBuffer,	// �R���X�^���g�o�b�t�@
			0,				// �R�s�[��T�u���\�[�X
			nullptr,		// �T�u���\�[�X���`����{�b�N�X�@�m�t�k�k�̏ꍇ�I�t�Z�b�g�Ȃ�
			&cb,			// �R�s�[���f�[�^
			0,				// 1�s�̃T�C�Y
			0);				// 1�[�x�X���C�X�̃T�C�Y

		// �萔�o�b�t�@3�𒸓_�V�F�[�_�[�փZ�b�g
		devcontext->VSSetConstantBuffers(
			3,				 // �X�^�[�g�X���b�g
			1,				 // ��
			&constantBuffer);// �R���X�^���g�o�b�t�@

		// �萔�o�b�t�@3���s�N�Z���V�F�[�_�[�փZ�b�g
		devcontext->PSSetConstantBuffers(3, 1, &constantBuffer);

		devcontext->DrawIndexed(modelDate->pmdMaterial[i].faceVertCount, idx, 0);			// ���f���`��
		idx += modelDate->pmdMaterial[i].faceVertCount;
	}
}
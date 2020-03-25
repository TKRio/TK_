//*****************************************************************************
//!	@file	Model.h
//!	@brief	PMX&PMD���f����\�����邽�߂̃N���X
//!	@note
//!	@author
//*****************************************************************************
#pragma once
#include "modelFileLoader.h"

//******************************************************************************
//class
//******************************************************************************
namespace MMDBreaker {
	class PMDModel {
	private:
		int				vertexCount;					//���_���̃J�E���g
		int				indexCount;						//�C���f�b�N�X�f�[�^�̃J�E���g
		int				colorCount;						//�F���̃J�E���g
		unsigned short *indexData;						//�C���f�b�N�X�f�[�^�̊i�[�ꏊ
		CBDATA          cb;								//�萔�o�b�t�@

		PMDData*				   modelDate = nullptr;		// ���f���f�[�^
		VERTEX*					   vertexData = nullptr;	// ���_�f�[�^
		ID3D11Buffer*			   vertexBuffer = nullptr;	// ���_�o�b�t�@
		ID3D11Buffer*              indexBuffer = nullptr;	// �C���f�b�N�X�o�b�t�@
		ID3D11Buffer*			   constantBuffer = nullptr;	// �R���X�^���g�o�b�t�@
		ID3D11VertexShader*		   vertexShader = nullptr;	// ���_�V�F�[�_�[
		ID3D11PixelShader*		   pixelShader = nullptr;	// �s�N�Z���V�F�[�_�[
		ID3D11InputLayout*		   vertexLayout = nullptr;	// ���_���C�A�E�g
		ID3D11SamplerState*        samplerState = nullptr;	// �T���v���X�e�[�g	
		ID3D11ShaderResourceView** srv = nullptr;	// �V�F�[�_�[���\�[�X�r���[

		ID3D11Device*				device;						// �f�o�C�X
		ID3D11DeviceContext*		devcontext;					//�f�o�C�X�R���e�L�X�g
	public:
		bool Init(const char* fileName, const char* vsfile, const char* psfile);
		void UnInit();
		void Draw();
		//void DrawParts();

		//Gettter
		PMDData* GetModelData()   { return modelDate;}
		VERTEX GetVertexData(int count) { return vertexData[count]; }
		int GetVertexCount()      {return vertexCount;}
		int GetColorCount()       { return colorCount; }
		unsigned short* GetIndexData() { return indexData;}
		int GetIndexCount() { return indexCount;}
	};
};
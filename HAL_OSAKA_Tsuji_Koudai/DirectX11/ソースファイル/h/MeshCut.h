//*****************************************************************************
//!	@file	MeshCut.h
//!	@brief	���b�V���J�b�g����
//!	@note	��񂾂��ؒf�\�B�e�N�X�`���̂��郂�f���͐؂�Ȃ��B�T�u�Z�b�g�����H
//!	@author
//*****************************************************************************

#pragma once
#include "Model.h"
#include "string"
#include "Mesh.h"

//*****************************************************************************
//Define
//*****************************************************************************
int const modelmax = 2;
int const planemax = 3;
int const posmax   = 3;
int const clonemax = 2;

//*****************************************************************************
//class
//*****************************************************************************

namespace MMDBreaker {
	class Cutter {
	private:
		Cutter() {};
		//�R�s�[�̋֎~
		Cutter(const Cutter &);
		//������Z�q�֎~
		void operator=(const Cutter &);

		PMDModel*				   model[modelmax];			//0 : �ؒfmesh 1:��Q��(targetobj)mesh 3: ����������mesh 
		std::vector<MESHDATA>	   clone[clonemax];			//�����������mesh���i�[���邽�߂̍\���̔z�� 0 :�@��@�P�F�@��
		std::vector<WORD>		   cloneIndex[clonemax];	//�C���f�b�N�X���
		std::vector<WORD>		   outSiderIndex[clonemax];	//�v�Z�ɂ͊֌W�̂Ȃ����_�̃C���f�b�N�X���
		std::vector<WORD>		   addToIndex[clonemax];	//�V���ɒǉ��������_�̃C���f�b�N�X���

		std::vector<MESHDATA>	   workMesh;				//work�p��Mesh�\����
		std::vector<WORD>		   workMeshIndex;			//�C���f�b�N�X���
		DirectX::XMFLOAT3		   pos[posmax];				//�I�u�W�F�N�g�Ɋ܂܂�Ă���O�p�`�|���S���̊e���_���𔲂��o��(1�|���S��3���_)	
		MESHDATA				   provi[clonemax];			//���̃f�[�^�����锠

		DirectX::XMFLOAT4X4 pMtx;						//player��matrix
		DirectX::XMFLOAT4X4 cMtx;						//cutter��matrix
		DirectX::XMFLOAT3 fixation;						//��Ɨp�i�����_�j
		int i;
		int colorCount;

		Mesh				afterMesh[clonemax];		//���b�V���J�b�g������̏������ĕ`�悷�� 0 : ��@�P�F��
		DirectX::XMFLOAT4X4 afterMtx[clonemax];
		DirectX::XMFLOAT3	afterAngle[clonemax];
		DirectX::XMFLOAT3	afterTrans[clonemax];

		VERTEX* overVertex = nullptr;
		VERTEX* underVertex = nullptr;
		//���ʂ̒�`
		Plane	  cuttingObjPlane;
		PLAVERTEX cuttingPlane;

		std::vector<int>		 workIndex;			//���[�N�p���_�z��
	public:
		//�V���O���g��
		static Cutter& getInstance() {
			static Cutter singleton;
			return singleton;
		}

		//�����̒��Ŏg�p����֐�
		DirectX::XMFLOAT3 ChangePoint(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4X4 mtx);			//���_�̃��[�J�����W�����[���h�ϊ�
		DirectX::XMFLOAT3 InverseChangePoint(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4X4 mtx);	//���[���h���W�����[�J�����W�ɕϊ�
		DirectX::XMFLOAT3 ChangeVector(DirectX::XMFLOAT3 pos);						//�x�N�^�[�����[���h���W�ɕϊ�
		DirectX::XMFLOAT3 Triangle(int count);
		DirectX::XMFLOAT3 Triangle(DirectX::XMFLOAT3 vert, DirectX::XMFLOAT4X4 mtx);

		void Init(PMDModel* cutter, PMDModel* target, DirectX::XMFLOAT4X4 cuttingMtx, DirectX::XMFLOAT4X4 targetMtx);
		void MeshCut();
		void UpDate(DirectX::XMFLOAT3 trans, DirectX::XMFLOAT3 angle);
		void Draw();
		void UnInit();

		//Getter
		DirectX::XMFLOAT4X4 GetAfterMeshWorldMtx(int count) { return afterMesh[count].GetWorldMtx(); };
		//Setter
		void SetAfterMeshWorldMtx(DirectX::XMFLOAT4X4 workMtx, int count) { afterMesh[count].SetWorldMtx(workMtx); };
		
	};
}

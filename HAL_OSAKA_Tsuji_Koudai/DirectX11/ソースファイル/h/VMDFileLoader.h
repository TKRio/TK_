//*****************************************************************************
//!	@file	VMDFileLoader.h
//!	@brief	VMDFile(���[�V�����t�@�C��)��ǂݍ��ނ��߂ɕK�v�ȃN���X
//!	@note	
//!	@author	
//*****************************************************************************
#pragma once
#include "dx11mathutil.h"

namespace VMDFileLoader {
	class VMD {
#pragma pack(push, 1)
	private:
		//VMD�w�b�_�[�\����
		typedef struct _Header_
		{
			char header[30];		//VMD0002
			char modelName[20];		//���f���l�[���H
		}VMDHeader;
		
		//���[�V�����f�[�^ 111bytes
		typedef struct _Motion_ {
			char				  BoneName[15];		//�{�[���̖��O
			unsigned long		  flameNo;			//�t���[���ԍ�
			DirectX::XMFLOAT3	  location;			//�ʒu
			DirectX::XMFLOAT4	  rotation;			//Quaternion��]
			char		  interpolation[64];		// [4][4][4]�@���(�p���H)
		}VMDMotion;

		//�\��f�[�^ 23bytes
		typedef struct _Skin_ {
			char			skinName[15];	//�X�L���l�[��
			unsigned long	flameNo;		//�t���[���ԍ�
			float			weight;			//�X�L���̐ݒ�l(�\��X���C�_�[�̒l)
		}VMDSkin;

		//�J�����f�[�^ 61bytes
		typedef struct _Camera_ {
			unsigned long flameNo;			//�t���[���ԍ�
			float		  length;			//-(����)
			DirectX::XMFLOAT3	  location;			//�ʒu
			DirectX::XMFLOAT3	  rotation;			//�I�C���[�p *X���͕������]���Ă���̂Œ���
			unsigned char interpolation[24];//[6][4](������) ���
			unsigned long viewingAngle;		//����p
			unsigned char perspective;		// 0:on 1:off �p�[�X�y�N�e�B�u
		}VMDCamera;

		//�Ɩ��f�[�^ 28bytes
		typedef struct _Light_ {
			unsigned long flameNo;			//�t���[���ԍ�
			DirectX::XMFLOAT3	  rgb;				//�FRGB
			DirectX::XMFLOAT3	  location;			//XYZ
		}VMDLight;

		//�Z���t�V���h�E�f�[�^	9bytes
		typedef struct _SelfShadow_ {
			unsigned long flameNo;			//�t���[���ԍ�
			unsigned char mode;				//00~02 ���[�h
			float		  Distance;			//0.1 - (dist * 0.00001) ����
		}VMDSelfShadow;

	public:
		VMDHeader				vmdHeader;
		VMDMotion				*vmdMotion = nullptr;
		VMDSkin					*vmdSkin   = nullptr;
		VMDCamera				*vmdCamera = nullptr;
		VMDLight				*vmdLight  = nullptr;
		VMDSelfShadow			*vmdSS	   = nullptr;

		unsigned int motionCount;
		unsigned int skinCount;
		unsigned int cameraCount;
		unsigned int lightCount;
		unsigned int selfShadowCount;
#pragma pack(pop)

		VMD(const char *fileName);
		VMD();
	};
}



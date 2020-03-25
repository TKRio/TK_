//*****************************************************************************
//!	@file	modelFileLoader.h
//!	@brief	PMX&PMDFile��ǂݍ���
//!	@note	
//!	@author	
//*****************************************************************************
#pragma once
#include "Plane.h"

//******************************************************************************
//class
//******************************************************************************
namespace MMDBreaker{
	class PMDData {
#pragma pack(push, 1)
	private:
		//�w�b�_�[�\����
		typedef struct _Header_ {
			unsigned char magic[3];
			float		  version;
			unsigned char modelName[20];
			unsigned char Comment[256];
		}PMDHEADER;

		//���_�\����
		typedef struct _Vertex_ {
			DirectX::XMFLOAT3 position;	// ���_���W
			DirectX::XMFLOAT3 normal;	// �@��
			DirectX::XMFLOAT2 uv;		// UV���W
			unsigned short boneNum[2];	//unsigned short = WORD
			unsigned char  boneWeinght;	//unsigned char = BYTE
			unsigned char  edgeFlag;
		}PMDVERTEX;

		//�ގ��\����(�}�e���A���f�[�^)
		typedef struct _Material_ {
			DirectX::XMFLOAT4	  diffuseColor;		//r,g,b,a
			float				  speclarity;			
			DirectX::XMFLOAT3	  specularColor;	//sr,sg,sb
			DirectX::XMFLOAT3	  mirrorColor;		//mr,mg,mb
			unsigned char toonIndex;
			unsigned char edgeFlag;
			unsigned long faceVertCount;
			char		  textureFileName[20];
		}PMDMATERIAL;

		//�{�[���f�[�^
		typedef struct _Bone_ {
			char			boneName[20];
			unsigned short	parentBoneIndex;	//�e�{�[���ԍ�
			unsigned short	tailPosBoneIndex;   //tail�ʒu�̃{�[���ԍ�
			unsigned char	boneType;
			unsigned short	ikParentBoneIndex;  //IK�{�[���ԍ�
			DirectX::XMFLOAT3 boneHeadPos;		//�{�[���w�b�h�̈ʒu
		}PMDBONE;

	public:
		PMDHEADER	pmdHeader;
		PMDVERTEX	*pmdVertex	 = nullptr;
		PMDMATERIAL *pmdMaterial = nullptr;
		PMDBONE		*pmdBone	 = nullptr;

		unsigned long  vertCount;				//���_���J�E���g unsigned long = DWORD
		unsigned long  faceVertCount;			//�ʂ̐����J�E���g
		unsigned long  materialCount;			//�}�e���A���̃J�E���g
		unsigned short boneCount;				//�{�[���̃J�E���g

		unsigned short *faceVertIndex;			//�ʂ̃f�[�^
#pragma pack(pop)

		//Getter
		//�ʒ��_���(index�o�b�t�@�̂悤�Ȃ���)
		unsigned short GetfaceVertIndex(int count) {
			return faceVertIndex[count];
		}

		unsigned long GetfaceVertCount() {
			return faceVertCount;
		}

		DirectX::XMFLOAT3 GetPmdVertexNormal(int count) {
			return pmdVertex[count].normal;
		}

		//���f���̒��_���̃|�W�V�������擾
		DirectX::XMFLOAT3 GetPmdVertexPos(int count) {
			return pmdVertex[count].position;
		}
		
		PMDData(const char* filenName);
		~PMDData();
	};
};


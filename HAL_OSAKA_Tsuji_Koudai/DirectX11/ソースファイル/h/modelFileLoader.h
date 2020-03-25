//*****************************************************************************
//!	@file	modelFileLoader.h
//!	@brief	PMX&PMDFileを読み込む
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
		//ヘッダー構造体
		typedef struct _Header_ {
			unsigned char magic[3];
			float		  version;
			unsigned char modelName[20];
			unsigned char Comment[256];
		}PMDHEADER;

		//頂点構造体
		typedef struct _Vertex_ {
			DirectX::XMFLOAT3 position;	// 頂点座標
			DirectX::XMFLOAT3 normal;	// 法線
			DirectX::XMFLOAT2 uv;		// UV座標
			unsigned short boneNum[2];	//unsigned short = WORD
			unsigned char  boneWeinght;	//unsigned char = BYTE
			unsigned char  edgeFlag;
		}PMDVERTEX;

		//材質構造体(マテリアルデータ)
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

		//ボーンデータ
		typedef struct _Bone_ {
			char			boneName[20];
			unsigned short	parentBoneIndex;	//親ボーン番号
			unsigned short	tailPosBoneIndex;   //tail位置のボーン番号
			unsigned char	boneType;
			unsigned short	ikParentBoneIndex;  //IKボーン番号
			DirectX::XMFLOAT3 boneHeadPos;		//ボーンヘッドの位置
		}PMDBONE;

	public:
		PMDHEADER	pmdHeader;
		PMDVERTEX	*pmdVertex	 = nullptr;
		PMDMATERIAL *pmdMaterial = nullptr;
		PMDBONE		*pmdBone	 = nullptr;

		unsigned long  vertCount;				//頂点数カウント unsigned long = DWORD
		unsigned long  faceVertCount;			//面の数をカウント
		unsigned long  materialCount;			//マテリアルのカウント
		unsigned short boneCount;				//ボーンのカウント

		unsigned short *faceVertIndex;			//面のデータ
#pragma pack(pop)

		//Getter
		//面頂点情報(indexバッファのようなもの)
		unsigned short GetfaceVertIndex(int count) {
			return faceVertIndex[count];
		}

		unsigned long GetfaceVertCount() {
			return faceVertCount;
		}

		DirectX::XMFLOAT3 GetPmdVertexNormal(int count) {
			return pmdVertex[count].normal;
		}

		//モデルの頂点情報のポジションを取得
		DirectX::XMFLOAT3 GetPmdVertexPos(int count) {
			return pmdVertex[count].position;
		}
		
		PMDData(const char* filenName);
		~PMDData();
	};
};


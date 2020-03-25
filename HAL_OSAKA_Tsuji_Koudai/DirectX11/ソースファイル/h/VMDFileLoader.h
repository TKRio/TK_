//*****************************************************************************
//!	@file	VMDFileLoader.h
//!	@brief	VMDFile(モーションファイル)を読み込むために必要なクラス
//!	@note	
//!	@author	
//*****************************************************************************
#pragma once
#include "dx11mathutil.h"

namespace VMDFileLoader {
	class VMD {
#pragma pack(push, 1)
	private:
		//VMDヘッダー構造体
		typedef struct _Header_
		{
			char header[30];		//VMD0002
			char modelName[20];		//モデルネーム？
		}VMDHeader;
		
		//モーションデータ 111bytes
		typedef struct _Motion_ {
			char				  BoneName[15];		//ボーンの名前
			unsigned long		  flameNo;			//フレーム番号
			DirectX::XMFLOAT3	  location;			//位置
			DirectX::XMFLOAT4	  rotation;			//Quaternion回転
			char		  interpolation[64];		// [4][4][4]　補間(姿勢？)
		}VMDMotion;

		//表情データ 23bytes
		typedef struct _Skin_ {
			char			skinName[15];	//スキンネーム
			unsigned long	flameNo;		//フレーム番号
			float			weight;			//スキンの設定値(表情スライダーの値)
		}VMDSkin;

		//カメラデータ 61bytes
		typedef struct _Camera_ {
			unsigned long flameNo;			//フレーム番号
			float		  length;			//-(距離)
			DirectX::XMFLOAT3	  location;			//位置
			DirectX::XMFLOAT3	  rotation;			//オイラー角 *X軸は符号反転しているので注意
			unsigned char interpolation[24];//[6][4](未検証) 補間
			unsigned long viewingAngle;		//視野角
			unsigned char perspective;		// 0:on 1:off パースペクティブ
		}VMDCamera;

		//照明データ 28bytes
		typedef struct _Light_ {
			unsigned long flameNo;			//フレーム番号
			DirectX::XMFLOAT3	  rgb;				//色RGB
			DirectX::XMFLOAT3	  location;			//XYZ
		}VMDLight;

		//セルフシャドウデータ	9bytes
		typedef struct _SelfShadow_ {
			unsigned long flameNo;			//フレーム番号
			unsigned char mode;				//00~02 モード
			float		  Distance;			//0.1 - (dist * 0.00001) 距離
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



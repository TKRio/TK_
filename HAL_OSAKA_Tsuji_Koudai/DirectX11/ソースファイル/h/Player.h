//*****************************************************************************
//!	@file	Player.h
//!	@brief	プレイヤーの処理
//!	@note	現在はカットされる側の処理
//!	@author
//*****************************************************************************
#pragma once
#include "CDirectInput.h"
#include "DX11Settransform.h"
#include "VMDMotion.h"
#include "MeshCut.h"
#include "updatespherecamera.h"

//******************************************************************************
//Define
//******************************************************************************
namespace charaDefine {
	const float speedMax = 2.0f;	//プレイヤーの移動の速さの最大値
	const float SpeedAdd = 0.1f;	//Playerの速さに足す数値
	const float rotateSpeed = 1.0f;	//Playerの回転速度
	const float Speed = 0.0f;		//Playerの移動の速さ
}

namespace cameraDefine {
	const int	cameraDelay = 25;
	const float fpsCameraPosEye = 100.0f;
	const float fpsCameraPosloockat = 200.0f;
	const float tpsCameraPosEye = 30.0f;
}

//モデルの数
const int pmdModelSize = 5;

//******************************************************************************
//構造体宣言
//******************************************************************************
typedef struct _cameraDate_ {

	DirectX::XMFLOAT3 axisZ3;						//進行方向ベクトル		//z軸
	DirectX::XMFLOAT3 eye;							//視点
	DirectX::XMFLOAT3 lookat;						//注視点
	DirectX::XMFLOAT3 up;							//カメラ上向きベクトル

	DirectX::XMFLOAT3 delay[cameraDefine::cameraDelay];//ディレイ

	float elevation;			//回転Y
	float azimuth;				//回転X
	float depth;				//奥行
	float radius;				//半径
}CAMERADATA;

typedef struct _ModelData_ {
	DirectX::XMFLOAT4X4	 matrix;	//モデルのmatrix
	DirectX::XMFLOAT3 angle;				// 角度情報
	DirectX::XMFLOAT3 trans;				// 平行移動量
}MODELDATA;

enum MODELNUMBER {
	e_Cube = 0,
	e_Sphere,
	e_Unknown,
	e_Hart,
	e_CubeVer2,
	NUMBER_MAX
};

//******************************************************************************
//class
//******************************************************************************

class Player {
private:
	MMDBreaker::PMDModel  victim[pmdModelSize];	//モデル格納
	MMDBreaker::PMDModel  cutter;
	MMDBreaker::PMDModel  skyDome;
	
	Motion::cVmd motion;		//モーションデータ格納	

	MODELDATA	 victimModelData[pmdModelSize];	//モデル情報
	MODELDATA	 cutterModelData;		//cutter
	MODELDATA	 skyDomeModelData;		//skyDome
	CAMERADATA	 cameraData;			//プレイヤーカメラ
	POINT		 p;						//マウスの座標を取得
	MODELNUMBER  modelNumber;

	int  num;
	bool isDraw;						//描画フラグ
public:
	bool Init();
	void InitMat();
	void UpDate();
	void Draw();
	void UnInit();

	//Getter
	const DirectX::XMFLOAT4X4 GetMatrix() { return cutterModelData.matrix;}
	const DirectX::XMFLOAT3 GetAngle() { return cutterModelData.angle;}
	const DirectX::XMFLOAT3 GetTrans() { return cutterModelData.trans;}	
	const CAMERADATA GetCameraData() { return cameraData;}

	//Setter
	void SetAngle(DirectX::XMFLOAT3 setAngle) { cutterModelData.angle = setAngle; }
	void SetTrans(DirectX::XMFLOAT3 setTrans) { cutterModelData.trans = setTrans;}
	void SetCameraDataRotate(float x,float y,float z) {
		cameraData.elevation = x;
		cameraData.azimuth	 = y;
		cameraData.depth = z;
	}

	void SetCameraDataPosition(float x, float y, float z) {
		cameraData.lookat.x = x;
		cameraData.lookat.y = y;
		cameraData.lookat.z = z;
	}
};
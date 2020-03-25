//*****************************************************************************
//!	@file	updatespherecamera.cpp
//!	@brief	円状カメラ処理
//!	@note	
//!	@author
//*****************************************************************************

#include <DirectXMath.h>
#include "Camera.h"

using namespace DirectX;

//角度をラジアンに変更
inline float ToRad(float angle) {
	return(angle * XM_PI) / 180.0f;
}

//-----------------------------------
// 球面束縛カメラ(Z軸回転なし)
//-----------------------------------
void UpdateSphereCamera(float radius,						//半径(入力)
						float elevation,					//仰角(入力)
						float azimuth,						//方位角(入力)
						XMFLOAT3& lookat)					//注視点(入力)
{
	XMFLOAT3 eye;									//視点
	//仰角、法威嚇をラジアンに変換
	elevation = ToRad(elevation);
	azimuth = ToRad(azimuth);

	//仰角に応じた半径を計算
	float r = radius * sinf(elevation);

	//視点座標を計算
	eye.x = r * cosf(azimuth);
	eye.y = radius * cosf(elevation);
	eye.z = r * sinf(azimuth);

	//上向きベクトルを計算(仰角を90度回転させるとできる)
	XMFLOAT3 up;
	float elevationplus90 = elevation + ToRad(90);

	r = radius * sinf(elevationplus90);

	up.x = r * cosf(azimuth);
	up.y = radius * cosf(elevationplus90);
	up.z = r * sinf(azimuth);

	//視点ベクトルを注視点分平行移動
	eye.x += lookat.x;
	eye.y += lookat.y;
	eye.z += lookat.z;

	//カメラのビュー変換行列の作成
	Camera::GetInstance()->SetEye(eye);
	Camera::GetInstance()->SetLookat(lookat);
	Camera::GetInstance()->SetUp(up);
	Camera::GetInstance()->CreateCameraMatrix();

}
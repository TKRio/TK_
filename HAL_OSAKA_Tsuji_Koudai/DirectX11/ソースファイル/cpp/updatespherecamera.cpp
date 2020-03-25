//*****************************************************************************
//!	@file	updatespherecamera.cpp
//!	@brief	�~��J��������
//!	@note	
//!	@author
//*****************************************************************************

#include <DirectXMath.h>
#include "Camera.h"

using namespace DirectX;

//�p�x�����W�A���ɕύX
inline float ToRad(float angle) {
	return(angle * XM_PI) / 180.0f;
}

//-----------------------------------
// ���ʑ����J����(Z����]�Ȃ�)
//-----------------------------------
void UpdateSphereCamera(float radius,						//���a(����)
						float elevation,					//�p(����)
						float azimuth,						//���ʊp(����)
						XMFLOAT3& lookat)					//�����_(����)
{
	XMFLOAT3 eye;									//���_
	//�p�A�@�Њd�����W�A���ɕϊ�
	elevation = ToRad(elevation);
	azimuth = ToRad(azimuth);

	//�p�ɉ��������a���v�Z
	float r = radius * sinf(elevation);

	//���_���W���v�Z
	eye.x = r * cosf(azimuth);
	eye.y = radius * cosf(elevation);
	eye.z = r * sinf(azimuth);

	//������x�N�g�����v�Z(�p��90�x��]������Ƃł���)
	XMFLOAT3 up;
	float elevationplus90 = elevation + ToRad(90);

	r = radius * sinf(elevationplus90);

	up.x = r * cosf(azimuth);
	up.y = radius * cosf(elevationplus90);
	up.z = r * sinf(azimuth);

	//���_�x�N�g���𒍎��_�����s�ړ�
	eye.x += lookat.x;
	eye.y += lookat.y;
	eye.z += lookat.z;

	//�J�����̃r���[�ϊ��s��̍쐬
	Camera::GetInstance()->SetEye(eye);
	Camera::GetInstance()->SetLookat(lookat);
	Camera::GetInstance()->SetUp(up);
	Camera::GetInstance()->CreateCameraMatrix();

}
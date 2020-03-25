//*****************************************************************************
//!	@file	Camera.cpp
//!	@brief	カメラの処理
//!	@note	インスタンス化されています
//!	@author
//*****************************************************************************

#include "Camera.h"

using namespace DirectX;

void Camera::Init(float nClip, float fClip, float fov,
	float width, float height,
	XMFLOAT3 eye, XMFLOAT3 lookat, XMFLOAT3 up) {

	SetProjection(nClip, fClip, fov, width, height);
	SetCamera(eye, lookat, up);
}

void Camera::CreateCameraMatrix() {
	ALIGN16 XMVECTOR Eye = XMVectorSet(eye.x, eye.y, eye.z, 0.0f);
	ALIGN16 XMVECTOR At = XMVectorSet(lookat.x, lookat.y, lookat.z, 0.0f);
	ALIGN16 XMVECTOR Up = XMVectorSet(up.x, up.y, up.z, 0.0f);
	ALIGN16 XMMATRIX camera;

	camera = XMMatrixLookAtLH(Eye, At, Up);

	XMStoreFloat4x4(&cameraMtx, camera);
}

void Camera::CreateProjectionMatrix() {

	ALIGN16 XMMATRIX projection;

	projection = XMMatrixPerspectiveFovLH(fov, aspect, nearClip, farClip);

	XMStoreFloat4x4(&projectionMtx, projection);
}

void Camera::ZoomIn(float magnification) {
	XMFLOAT3 direction;
	direction.x = lookat.x - eye.x;
	direction.y = lookat.y - eye.y;
	direction.z = lookat.z - eye.z;

	DX11Vec3Normalize(direction, direction);

	eye.x += direction.x*magnification;
	eye.y += direction.y*magnification;
	eye.z += direction.z*magnification;

	CreateCameraMatrix();
}

void Camera::ZoomOut(float magnification) {
	XMFLOAT3 direction;
	direction.x = lookat.x - eye.x;
	direction.z = lookat.z - eye.z;
	direction.y = lookat.y - eye.y;

	DX11Vec3Normalize(direction, direction);

	eye.x -= direction.x*magnification;
	eye.y -= direction.y*magnification;
	eye.z -= direction.z*magnification;

	CreateCameraMatrix();
}
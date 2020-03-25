//*****************************************************************************
//!	@file	Camera.h
//!	@brief	カメラの処理
//!	@note	インスタンス化されています
//!	@author
//*****************************************************************************
#pragma once
#include	<directxmath.h>
#include	"Memory.h"
#include	"dx11mathutil.h"

//*****************************************************************************
//class
//*****************************************************************************
class Camera{
private:
	DirectX::XMFLOAT4X4		projectionMtx;
	DirectX::XMFLOAT4X4		cameraMtx;

	DirectX::XMFLOAT3		eye;			// カメラ位置
	DirectX::XMFLOAT3		lookat;			// 注視点
	DirectX::XMFLOAT3		up;				// 上向きベクトル

	float			nearClip;
	float			aspect;
	float			fov;
	float			farClip;

public:
	//インスタンス化
	Camera() {
	}
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&&) = delete;
	Camera& operator=(Camera&&) = delete;

	static Camera* GetInstance(){
		static Camera instance;
		return &instance;
	}
	
	void Init(float nClip, float fClip, float fov,float width, float height,
			  DirectX::XMFLOAT3 eye, DirectX::XMFLOAT3 lookat, DirectX::XMFLOAT3 up);
	
	//Setter
	void SetNear(float nClip){ nearClip = nClip;}
	void SetFar(float fClip)  { farClip = fClip;}
	void SetFov(float viewingAngle)	{ fov = viewingAngle;}
	void SetAspect( float width,float height){ aspect = width/height;}
	void SetEye( DirectX::XMFLOAT3& eyePos) { eye = eyePos; }
	void SetLookat( DirectX::XMFLOAT3& lookatPos) { lookat = lookatPos; }
	void SetUp( DirectX::XMFLOAT3& upPos) { up = upPos; }

	void SetProjection(float nearclip,float farclip,float fov,float width,float height){
		SetNear(nearclip);
		SetFar(farclip);
		SetFov(fov);
		SetAspect(width,height);
		CreateProjectionMatrix();
	}
		
	void SetCamera(DirectX::XMFLOAT3& eye, DirectX::XMFLOAT3& lookat, DirectX::XMFLOAT3& up){
		SetEye(eye);
		SetLookat(lookat);
		SetUp(up);
		CreateCameraMatrix();
	}

	//Getter
	DirectX::XMFLOAT4X4& GetCameraMatrix()     { return cameraMtx;}
	DirectX::XMFLOAT4X4& GetProjectionMatrix() { return projectionMtx;}
	float				 GetFov()    { return fov;}
	DirectX::XMFLOAT3&   GetEye()    { return eye;}
	DirectX::XMFLOAT3&   GetLookat() { return lookat;}
	DirectX::XMFLOAT3&   GetUp()     { return up;}

	void CreateCameraMatrix();		
	void CreateProjectionMatrix();

	void ZoomIn(float magnification);
	void ZoomOut(float magnification);
};
#pragma once
#include <DirectXMath.h>

void UpdateSphereCamera(float radius,						//���a(����)
	float elevation,					//�p(����)
	float azimuth,						//���ʊp(����)
	DirectX::XMFLOAT3& lookat);			//�����_(����)
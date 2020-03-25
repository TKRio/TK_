#pragma once
#include "dx11mathutil.h"

namespace MMDBreaker {
	class Plane {
	private:
		PLAVERTEX			 planePos;
		DirectX::XMFLOAT3    planeCenter;
		DirectX::XMFLOAT3    normal;

	public:
		PLAVERTEX CreatePlaneFromPorigon(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 c);					//polygon�̏�񂩂畽�ʂ��쐬����
		PLAVERTEX CreatePlaneFromPointNormal(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 normal);								//1���_�Ɩ@����񂩂畽�ʂ��쐬����i�������j

		bool IntersectionPlaneAndLine(DirectX::XMFLOAT3* ansVector, DirectX::XMFLOAT3 A, DirectX::XMFLOAT3 B, PLAVERTEX PL);		//��_�����߂�
		bool GetSide(DirectX::XMFLOAT3 p);

		DirectX::XMFLOAT3 GetNormal() {
			return normal;
		}

		DirectX::XMFLOAT3 GetCenter() {
			return planeCenter;
		}
	};
};

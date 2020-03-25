#pragma once
#include "dx11mathutil.h"

namespace MMDBreaker {
	class Plane {
	private:
		PLAVERTEX			 planePos;
		DirectX::XMFLOAT3    planeCenter;
		DirectX::XMFLOAT3    normal;

	public:
		PLAVERTEX CreatePlaneFromPorigon(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 c);					//polygonの情報から平面を作成する
		PLAVERTEX CreatePlaneFromPointNormal(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 normal);								//1頂点と法線情報から平面を作成する（無限遠）

		bool IntersectionPlaneAndLine(DirectX::XMFLOAT3* ansVector, DirectX::XMFLOAT3 A, DirectX::XMFLOAT3 B, PLAVERTEX PL);		//交点を求める
		bool GetSide(DirectX::XMFLOAT3 p);

		DirectX::XMFLOAT3 GetNormal() {
			return normal;
		}

		DirectX::XMFLOAT3 GetCenter() {
			return planeCenter;
		}
	};
};

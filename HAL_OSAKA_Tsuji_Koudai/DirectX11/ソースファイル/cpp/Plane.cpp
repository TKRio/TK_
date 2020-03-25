#include "Plane.h"
#include	<DirectXMath.h>
#include	"CDirectxGraphics.h"
#include	"shader.h"

///////////////////////////////////////////////
//平面を作成する
///////////////////////////////////////////////

using namespace DirectX;

//一頂点と法線ベクトルから平面を作成する
PLAVERTEX MMDBreaker::Plane::CreatePlaneFromPointNormal(XMFLOAT3 p, XMFLOAT3 nor) {

	float d;
	DX11Vec3Dot(d, p, nor);

	planePos.a = p.x;
	planePos.b = p.y;
	planePos.c = p.z;
	planePos.d = -d;

	planeCenter = XMFLOAT3(planePos.a * planePos.d, planePos.d * planePos.d, planePos.c * planePos.d);

	return planePos;
}

//polygonから平面を求める
PLAVERTEX MMDBreaker::Plane::CreatePlaneFromPorigon(XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c) {

	XMFLOAT3 ab = XMFLOAT3Arithmetic(b, a, 1);
	XMFLOAT3 bc = XMFLOAT3Arithmetic(c, b, 1);
	//外積を求める（法線ベクトル）
	DX11Vec3Cross(normal, ab, bc);
	DX11Vec3Normalize(normal, normal);

	//1頂点から法線を基に平面を作成する
	return CreatePlaneFromPointNormal(a, normal);
}


///////////////////////////////////////////////
//平面から見て、頂点はどちらに存在しているのか
///////////////////////////////////////////////
bool MMDBreaker::Plane::GetSide(XMFLOAT3 p) {
	//頂点座標　法線情報(どちらを向いているか)　が必要
	//負の向きか正の向きかを法線ベクトルから求める
	// 戻り値    0:表    1:裏    -1:エラー

	//頂点ｐとの内積を取る
	float d;
	DX11Vec3Dot(d, p, normal);

	if (d < 0.0) {
		return true;		//ポリゴンはベクトルvから見て表側
	}
	else
		if (d > 0.0) {
			return false;    //ポリゴンはベクトルvから見て裏側
		}
		else
			if (d == 0.0f) {
				return true;		//同一線上であれば、一応表側判定しておく
			}
	return 0;
}

/////////////////////////////////////////////////
//平面と線分の交点を求める:Plane側の関数
////////////////////////////////////////////////
bool MMDBreaker::Plane::IntersectionPlaneAndLine(XMFLOAT3* ansVector,		//答えを格納する
	XMFLOAT3 A,					//頂点A
	XMFLOAT3 B,					//頂点B
	PLAVERTEX PL)					//平面mesh
{
	//中心点を求める
	XMFLOAT3 P = XMFLOAT3(PL.a * PL.d, PL.b * PL.d, PL.c * PL.d);

	//PA PBベクトル
	XMFLOAT3 PA = XMFLOAT3(P.x - A.x, P.y - A.y, P.z - A.z);
	XMFLOAT3 PB = XMFLOAT3(P.x - B.x, P.y - B.y, P.z - B.z);

	//PA PBそれぞれを平面法線と内積
	double dot_PA = PA.x * PL.a + PA.y * PL.b + PA.z * PL.c;
	double dot_PB = PB.x * PL.a + PB.y * PL.b + PB.z * PL.c;

	//これは線端が平面上にあった時の計算の誤差を吸収しています。調整して使ってください。
	if (abs(dot_PA) < 0.000001) { dot_PA = 0.0; }
	if (abs(dot_PB) < 0.000001) { dot_PB = 0.0; }

	//交差判定
	if (dot_PA == 0.0 && dot_PB == 0.0) {
		//両端が平面上にあり、交点を計算できない。
		return true;
	}
	else
		if ((dot_PA >= 0.0 && dot_PB <= 0.0) ||
			(dot_PA <= 0.0 && dot_PB >= 0.0)) {
			//内積の片方がプラスで片方がマイナスなので、交差している

		}
		else {
			//交差していない
			return false;
		}

	//以下、交点を求める 

	XMFLOAT3 AB = XMFLOAT3(B.x - A.x, B.y - A.y, B.z - A.z);

	//交点とAの距離 : 交点とBの距離 = dot_PA : dot_PB
	float hiritu = static_cast<float>(abs(dot_PA) / (abs(dot_PA) + abs(dot_PB)));

	ansVector->x = A.x + (AB.x * hiritu);
	ansVector->y = A.y + (AB.y * hiritu);
	ansVector->z = A.z + (AB.z * hiritu);

	return true;
}

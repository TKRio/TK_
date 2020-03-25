#pragma once
#include	<DirectXMath.h>

//Plane情報構造体
typedef struct _PLANE_ {
	float a, b, c, d;
}PLAVERTEX;

// 定数バッファ定義（マテリアル単位）
typedef struct _ConstantBuffer_ {
	DirectX::XMFLOAT4	AmbientMaterial;		// 環境光の反射率
	DirectX::XMFLOAT4	DiffuseMaterial;		// ディフューズ光の反射率
	DirectX::XMFLOAT4	SpecularMaterial;		// スペキュラ光の反射率
}CBDATA;

//頂点情報構造体
typedef struct _Vertex_ {
	DirectX::XMFLOAT3 position;	// 頂点座標
	DirectX::XMFLOAT3 nomal;	// 頂点座標
	DirectX::XMFLOAT4 col;		// カラー
	DirectX::XMFLOAT2 uv;		// UV座標
}VERTEX;

void DX11MakeWorldMatrix(DirectX::XMFLOAT4X4& mat, DirectX::XMFLOAT3& angle, DirectX::XMFLOAT3& trans);
void DX11Vec3MulMatrix(DirectX::XMFLOAT3& outvec, const DirectX::XMFLOAT3& invec, const DirectX::XMFLOAT4X4& mat);
void DX11Vec3Cross(DirectX::XMFLOAT3& outvec, DirectX::XMFLOAT3& invec1, DirectX::XMFLOAT3& invec2);
void DX11Vec3Dot(float& dot, const DirectX::XMFLOAT3& invec1, const DirectX::XMFLOAT3& invec2);
void DX11Vec3Normalize(DirectX::XMFLOAT3& outvec, DirectX::XMFLOAT3& invec);
void DX11MtxIdentity(DirectX::XMFLOAT4X4& mat);
void DX11GetQtfromMatrix(const DirectX::XMFLOAT4X4& mat, DirectX::XMFLOAT4& qt);
void DX11QtRotationAxis(DirectX::XMFLOAT4& qt, DirectX::XMFLOAT4& axis, float angle);
void DX11QtMul(DirectX::XMFLOAT4& outqt, const DirectX::XMFLOAT4& qt1, const DirectX::XMFLOAT4& qt);
void DX11MtxFromQt(DirectX::XMFLOAT4X4& outmtx, const DirectX::XMFLOAT4& qt);
void DX11QtNormalize(DirectX::XMFLOAT4& inqt, DirectX::XMFLOAT4& outqt);
void DX11Vec4Length(DirectX::XMFLOAT4& vec, float& length);
void DX11p2pLength(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2, float& length);
void DX11MtxTranslation(const DirectX::XMFLOAT3& trans, DirectX::XMFLOAT4X4& matrix);
void DX11QtSlerp(const DirectX::XMFLOAT4& fromqt, const DirectX::XMFLOAT4& toqt, float t, DirectX::XMFLOAT4& ansqt);
void DX11QtIdentity(DirectX::XMFLOAT4& qt);
void DX11Vec3Length(DirectX::XMFLOAT3& vec, float& length);
void DX11MtxRotationAxis(const DirectX::XMFLOAT3& axis, float angle, DirectX::XMFLOAT4X4& outmtx);
void DX11MtxMultiply(DirectX::XMFLOAT4X4& ansmtx, const DirectX::XMFLOAT4X4& p1mtx, const DirectX::XMFLOAT4X4& p2mtx);

bool IsInFrustum(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4X4& matrix);
void IsInFrustum(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4X4& matrix, float ans[]);
bool IsInFrustum(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4X4& matrix, float ans[], PLAVERTEX* plane);
bool IsInFrustum(
	const DirectX::XMFLOAT3& pos,			// BS中心座標
	float radius,							// 半径
	const DirectX::XMFLOAT4X4& matrix,		// ビュー変換行列＊プロジェクション変換行列 
	float ans[],							// 平面との距離
	bool  canseeflag[],						// フラスタム平面とＢＳの関係
	PLAVERTEX* plane);							// 平面方程式;

void  GetPlanefromProjectionmatrix(PLAVERTEX* p, const DirectX::XMFLOAT4X4& matrix);
void  GetPlanefromProjectionmatrixWithNormalize(PLAVERTEX* p, const DirectX::XMFLOAT4X4& matrix);

void  DX11MtxInverse(DirectX::XMFLOAT4X4& ansmtx, const DirectX::XMFLOAT4X4& mtx);
void  DX11MtxTranspose(DirectX::XMFLOAT4X4& outmtx, const DirectX::XMFLOAT4X4& inmtx);
void  DX11MtxRotationX(float angle, DirectX::XMFLOAT4X4& outmtx);
void  DX11MtxRotationY(float angle, DirectX::XMFLOAT4X4& outmtx);
void  DX11MtxRotationZ(float angle, DirectX::XMFLOAT4X4& outmtx);
void  DX11MtxScale(float sx, float sy, float sz, DirectX::XMFLOAT4X4& outmtx);
float haltonseq(unsigned int no, unsigned int base);
void DX11MatrixIdentity(DirectX::XMFLOAT4X4& mat);

DirectX::XMFLOAT3 XMFLOAT3Arithmetic(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b, int count);	//XMFLOAT3の四則演算
bool XMFLOAT3Comparison(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b, int count);	//XMFLOAT3の比較

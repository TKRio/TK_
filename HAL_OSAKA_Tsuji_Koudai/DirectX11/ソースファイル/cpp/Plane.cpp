#include "Plane.h"
#include	<DirectXMath.h>
#include	"CDirectxGraphics.h"
#include	"shader.h"

///////////////////////////////////////////////
//���ʂ��쐬����
///////////////////////////////////////////////

using namespace DirectX;

//�꒸�_�Ɩ@���x�N�g�����畽�ʂ��쐬����
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

//polygon���畽�ʂ����߂�
PLAVERTEX MMDBreaker::Plane::CreatePlaneFromPorigon(XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c) {

	XMFLOAT3 ab = XMFLOAT3Arithmetic(b, a, 1);
	XMFLOAT3 bc = XMFLOAT3Arithmetic(c, b, 1);
	//�O�ς����߂�i�@���x�N�g���j
	DX11Vec3Cross(normal, ab, bc);
	DX11Vec3Normalize(normal, normal);

	//1���_����@������ɕ��ʂ��쐬����
	return CreatePlaneFromPointNormal(a, normal);
}


///////////////////////////////////////////////
//���ʂ��猩�āA���_�͂ǂ���ɑ��݂��Ă���̂�
///////////////////////////////////////////////
bool MMDBreaker::Plane::GetSide(XMFLOAT3 p) {
	//���_���W�@�@�����(�ǂ���������Ă��邩)�@���K�v
	//���̌��������̌�������@���x�N�g�����狁�߂�
	// �߂�l    0:�\    1:��    -1:�G���[

	//���_���Ƃ̓��ς����
	float d;
	DX11Vec3Dot(d, p, normal);

	if (d < 0.0) {
		return true;		//�|���S���̓x�N�g��v���猩�ĕ\��
	}
	else
		if (d > 0.0) {
			return false;    //�|���S���̓x�N�g��v���猩�ė���
		}
		else
			if (d == 0.0f) {
				return true;		//�������ł���΁A�ꉞ�\�����肵�Ă���
			}
	return 0;
}

/////////////////////////////////////////////////
//���ʂƐ����̌�_�����߂�:Plane���̊֐�
////////////////////////////////////////////////
bool MMDBreaker::Plane::IntersectionPlaneAndLine(XMFLOAT3* ansVector,		//�������i�[����
	XMFLOAT3 A,					//���_A
	XMFLOAT3 B,					//���_B
	PLAVERTEX PL)					//����mesh
{
	//���S�_�����߂�
	XMFLOAT3 P = XMFLOAT3(PL.a * PL.d, PL.b * PL.d, PL.c * PL.d);

	//PA PB�x�N�g��
	XMFLOAT3 PA = XMFLOAT3(P.x - A.x, P.y - A.y, P.z - A.z);
	XMFLOAT3 PB = XMFLOAT3(P.x - B.x, P.y - B.y, P.z - B.z);

	//PA PB���ꂼ��𕽖ʖ@���Ɠ���
	double dot_PA = PA.x * PL.a + PA.y * PL.b + PA.z * PL.c;
	double dot_PB = PB.x * PL.a + PB.y * PL.b + PB.z * PL.c;

	//����͐��[�����ʏ�ɂ��������̌v�Z�̌덷���z�����Ă��܂��B�������Ďg���Ă��������B
	if (abs(dot_PA) < 0.000001) { dot_PA = 0.0; }
	if (abs(dot_PB) < 0.000001) { dot_PB = 0.0; }

	//��������
	if (dot_PA == 0.0 && dot_PB == 0.0) {
		//���[�����ʏ�ɂ���A��_���v�Z�ł��Ȃ��B
		return true;
	}
	else
		if ((dot_PA >= 0.0 && dot_PB <= 0.0) ||
			(dot_PA <= 0.0 && dot_PB >= 0.0)) {
			//���ς̕Е����v���X�ŕЕ����}�C�i�X�Ȃ̂ŁA�������Ă���

		}
		else {
			//�������Ă��Ȃ�
			return false;
		}

	//�ȉ��A��_�����߂� 

	XMFLOAT3 AB = XMFLOAT3(B.x - A.x, B.y - A.y, B.z - A.z);

	//��_��A�̋��� : ��_��B�̋��� = dot_PA : dot_PB
	float hiritu = static_cast<float>(abs(dot_PA) / (abs(dot_PA) + abs(dot_PB)));

	ansVector->x = A.x + (AB.x * hiritu);
	ansVector->y = A.y + (AB.y * hiritu);
	ansVector->z = A.z + (AB.z * hiritu);

	return true;
}

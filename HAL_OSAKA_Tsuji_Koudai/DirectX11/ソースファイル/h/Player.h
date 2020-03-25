//*****************************************************************************
//!	@file	Player.h
//!	@brief	�v���C���[�̏���
//!	@note	���݂̓J�b�g����鑤�̏���
//!	@author
//*****************************************************************************
#pragma once
#include "CDirectInput.h"
#include "DX11Settransform.h"
#include "VMDMotion.h"
#include "MeshCut.h"
#include "updatespherecamera.h"

//******************************************************************************
//Define
//******************************************************************************
namespace charaDefine {
	const float speedMax = 2.0f;	//�v���C���[�̈ړ��̑����̍ő�l
	const float SpeedAdd = 0.1f;	//Player�̑����ɑ������l
	const float rotateSpeed = 1.0f;	//Player�̉�]���x
	const float Speed = 0.0f;		//Player�̈ړ��̑���
}

namespace cameraDefine {
	const int	cameraDelay = 25;
	const float fpsCameraPosEye = 100.0f;
	const float fpsCameraPosloockat = 200.0f;
	const float tpsCameraPosEye = 30.0f;
}

//���f���̐�
const int pmdModelSize = 5;

//******************************************************************************
//�\���̐錾
//******************************************************************************
typedef struct _cameraDate_ {

	DirectX::XMFLOAT3 axisZ3;						//�i�s�����x�N�g��		//z��
	DirectX::XMFLOAT3 eye;							//���_
	DirectX::XMFLOAT3 lookat;						//�����_
	DirectX::XMFLOAT3 up;							//�J����������x�N�g��

	DirectX::XMFLOAT3 delay[cameraDefine::cameraDelay];//�f�B���C

	float elevation;			//��]Y
	float azimuth;				//��]X
	float depth;				//���s
	float radius;				//���a
}CAMERADATA;

typedef struct _ModelData_ {
	DirectX::XMFLOAT4X4	 matrix;	//���f����matrix
	DirectX::XMFLOAT3 angle;				// �p�x���
	DirectX::XMFLOAT3 trans;				// ���s�ړ���
}MODELDATA;

enum MODELNUMBER {
	e_Cube = 0,
	e_Sphere,
	e_Unknown,
	e_Hart,
	e_CubeVer2,
	NUMBER_MAX
};

//******************************************************************************
//class
//******************************************************************************

class Player {
private:
	MMDBreaker::PMDModel  victim[pmdModelSize];	//���f���i�[
	MMDBreaker::PMDModel  cutter;
	MMDBreaker::PMDModel  skyDome;
	
	Motion::cVmd motion;		//���[�V�����f�[�^�i�[	

	MODELDATA	 victimModelData[pmdModelSize];	//���f�����
	MODELDATA	 cutterModelData;		//cutter
	MODELDATA	 skyDomeModelData;		//skyDome
	CAMERADATA	 cameraData;			//�v���C���[�J����
	POINT		 p;						//�}�E�X�̍��W���擾
	MODELNUMBER  modelNumber;

	int  num;
	bool isDraw;						//�`��t���O
public:
	bool Init();
	void InitMat();
	void UpDate();
	void Draw();
	void UnInit();

	//Getter
	const DirectX::XMFLOAT4X4 GetMatrix() { return cutterModelData.matrix;}
	const DirectX::XMFLOAT3 GetAngle() { return cutterModelData.angle;}
	const DirectX::XMFLOAT3 GetTrans() { return cutterModelData.trans;}	
	const CAMERADATA GetCameraData() { return cameraData;}

	//Setter
	void SetAngle(DirectX::XMFLOAT3 setAngle) { cutterModelData.angle = setAngle; }
	void SetTrans(DirectX::XMFLOAT3 setTrans) { cutterModelData.trans = setTrans;}
	void SetCameraDataRotate(float x,float y,float z) {
		cameraData.elevation = x;
		cameraData.azimuth	 = y;
		cameraData.depth = z;
	}

	void SetCameraDataPosition(float x, float y, float z) {
		cameraData.lookat.x = x;
		cameraData.lookat.y = y;
		cameraData.lookat.z = z;
	}
};
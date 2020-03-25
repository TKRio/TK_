//*****************************************************************************
//!	@file	Player.cpp
//!	@brief	プレイヤー処理纏め
//!	@note	
//!	@author	
//*****************************************************************************
#include "Player.h"
#include <windows.h>
#include "MouseInput.h"


bool Player::Init() {
	for (int i = 0; i < pmdModelSize; i++) {
		victimModelData[i].angle = { 0.0f,0.0f,0.0f };		//回転角度初期化
		victimModelData[i].trans = { 0.0f,0.0f,0.0f };		//位置情報初期化
		DX11MtxIdentity(victimModelData[i].matrix);			//姿勢の初期化
	}

	cutterModelData.angle = { 0.0f,0.0f,0.0f };
	cutterModelData.trans = { 0.0f,0.0f,0.0f };
	DX11MtxIdentity(cutterModelData.matrix);

	skyDomeModelData.angle = { 0.0f,90.0f,0.0f };
	skyDomeModelData.trans = { 0.0f,0.0f,0.0f };
	DX11MtxIdentity(skyDomeModelData.matrix);


	cameraData.elevation = -90.0f;
	cameraData.azimuth	 =  90.0f;
	cameraData.depth = cameraDefine::tpsCameraPosEye;

	bool sts;
	//スカイドーム
	sts = skyDome.Init("assets/sora.pmd", "shader/vs.fx", "shader/psskydome.fx");
	if (!sts) {
		MessageBox(nullptr, "Model Initエラー", "error!!", MB_OK);
		return false;
	}

	//切る側のobject
	sts = cutter.Init("assets/Tile.pmd", "shader/vs.fx", "shader/psskydome.fx");
	if (!sts) {
		MessageBox(nullptr, "Model Initエラー", "error!!", MB_OK);
		return false;
	}

	//モデルの初期化 切られる側のobject
	sts = victim[0].Init("assets/Cube_ver3.pmd", "shader/vs.fx", "shader/ps.fx");
	if (!sts) {
		MessageBox(nullptr, "Model Initエラー", "error!!", MB_OK);
		return false;
	}

	sts = victim[1].Init("assets/Sphere.pmd", "shader/vs.fx", "shader/ps.fx");
	if (!sts) {
		MessageBox(nullptr, "Model Initエラー", "error!!", MB_OK);
		return false;
	}

	sts = victim[2].Init("assets/isu.pmd", "shader/vs.fx", "shader/ps.fx");
	if (!sts) {
		MessageBox(nullptr, "Model Initエラー", "error!!", MB_OK);
		return false;
	}

	sts = victim[3].Init("assets/hart.pmd", "shader/vs.fx", "shader/ps.fx");
	if (!sts) {
		MessageBox(nullptr, "Model Initエラー", "error!!", MB_OK);
		return false;
	}

	sts = victim[4].Init("assets/miku3.pmd", "shader/vs.fx", "shader/ps.fx");
	if (!sts) {
		MessageBox(nullptr, "Model Initエラー", "error!!", MB_OK);
		return false;
	}

	//animationデータの初期化
	sts = motion.Init("motion/基本_構え_キックボクシング_000.vmd");
	if (!sts) {
		MessageBox(nullptr, "Motion Initエラー", "error!!", MB_OK);
		return false;
	}

	modelNumber = e_Cube;
	num = 0;
	isDraw = true;

	return true;
}

void Player::InitMat() {

	for (int i = 0; i < pmdModelSize; i++) {
		victimModelData[i].angle = { 0.0f,0.0f,0.0f };		//回転角度初期化
		victimModelData[i].trans = { 0.0f,0.0f,0.0f };		//位置情報初期化
		DX11MtxIdentity(victimModelData[i].matrix);			//姿勢の初期化
	}

	DX11MtxIdentity(cutterModelData.matrix);
	cutterModelData.angle = { 0.0f,90.0f,0.0f };
	cutterModelData.trans = { 0.0f,0.0f,0.0f };

	skyDomeModelData.angle = { 0.0f,0.0f,0.0f };
	skyDomeModelData.trans = { 0.0f,0.0f,0.0f };
	DX11MtxIdentity(skyDomeModelData.matrix);

	modelNumber = e_Cube;

	cameraData.elevation = -90.0f;
	cameraData.azimuth = 90.0f;
	cameraData.depth = cameraDefine::tpsCameraPosEye;
	isDraw = true;
}

void Player::UpDate() {
	
	cameraData.axisZ3.x = cutterModelData.matrix._31;
	cameraData.axisZ3.y = cutterModelData.matrix._32;
	cameraData.axisZ3.z = cutterModelData.matrix._33;

	switch (modelNumber)
	{
	case e_Cube:
		num = 0;
		break;
	case e_Sphere:
		num = 1;
		break;
	case e_Unknown:
		num = 2;
		break;
	case e_Hart:
		num = 3;
		break;
	case e_CubeVer2:
		num = 4;
		break;

	default:
		break;
	}
	//モデルチェンジ
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_1)) modelNumber = e_Cube;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_2)) modelNumber = e_Sphere;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_3)) modelNumber = e_Unknown;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_4)) modelNumber = e_Hart;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_5)) modelNumber = e_CubeVer2;
	
	//移動
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_Z)) victimModelData[num].trans.z += 0.1f;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_C)) victimModelData[num].trans.z -= 0.1f;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) victimModelData[num].trans.x -= 0.1f;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D)) victimModelData[num].trans.x += 0.1f;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) victimModelData[num].trans.y += 0.1f;
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S)) victimModelData[num].trans.y -= 0.1f;

	//パネルの移動
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_Z)) cutterModelData.trans.z += 0.1f;
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_C)) cutterModelData.trans.z -= 0.1f;
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) cutterModelData.trans.x -= 0.1f;
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D)) cutterModelData.trans.x += 0.1f;
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) cutterModelData.trans.y += 0.1f;
	//if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S)) cutterModelData.trans.y -= 0.1f;
	
	//----------------------------------
	//キャラクターを中心に回転するカメラ
	//---------------------------------
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LSHIFT)) {
		//	//モデルの回転
			if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT))	victimModelData[num].angle.y += 0.5f;
			if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT))	victimModelData[num].angle.y -= 0.5f;
			if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_UP))		victimModelData[num].angle.x += 0.5f;
			if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_DOWN))	victimModelData[num].angle.x -= 0.5f;

		//	//パネルの回転
		//	/*if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT))	cutterModelData.angle.y += 0.5f;
		//	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT))	cutterModelData.angle.y -= 0.5f;
		//	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_UP))		cutterModelData.angle.x += 0.5f;
		//	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_DOWN))	cutterModelData.angle.x -= 0.5f;*/
		//}

		//マウスホイールの値変動でズームイン＆ズームアウト
		if (Mouse::GetInstance()->GetWheelUpDown() > 0) {
			cameraData.depth += charaDefine::rotateSpeed*2;
			Mouse::GetInstance()->SetWheelUpDown(0);
		}
		else if (Mouse::GetInstance()->GetWheelUpDown() < 0) {
			cameraData.depth -= charaDefine::rotateSpeed;
			if (cameraData.depth <= 1) {
				cameraData.depth = 1.0f;
			}
			Mouse::GetInstance()->SetWheelUpDown(0);
		}

		cameraData.lookat = {
		  cutterModelData.matrix._41,
		  cutterModelData.matrix._42,
		  cutterModelData.matrix._43
		};
	}
	else {
		//-------------------------------------------
		//カメラの移動
		//-------------------------------------------
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_UP))	   cameraData.elevation -= 0.5f;
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_DOWN))  cameraData.elevation += 0.5f;
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT))  cameraData.azimuth   += 0.5f;
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT)) cameraData.azimuth   -= 0.5f;
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_F1))    cameraData.depth		+= 0.5f;
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_F2))    cameraData.depth		-= 0.5f;

		if (cameraData.depth < 0.1) {
			cameraData.depth = 0.1f;
		}

		if (Mouse::GetInstance()->GetRButtonClick()) {
			/*Todo クリックしたwinidow座標を取得し、その場所から奥に向けて板ポリを生成する*/

		}
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RETURN) && isDraw) {
		isDraw = false;
		MMDBreaker::Cutter::getInstance().UnInit();
		//MeshCutInit
		MMDBreaker::Cutter::getInstance().Init(&cutter, &victim[num], cutterModelData.matrix, victimModelData[num].matrix); //テスト段階の為、モデルデータは適当
		//メッシュカット
		MMDBreaker::Cutter::getInstance().MeshCut();
	}

	//カメラの更新
	UpdateSphereCamera(cameraData.depth, cameraData.elevation, cameraData.azimuth, cameraData.lookat);
	DX11MakeWorldMatrix(victimModelData[num].matrix, victimModelData[num].angle, victimModelData[num].trans);
	DX11MakeWorldMatrix(cutterModelData.matrix, cutterModelData.angle, cutterModelData.trans);

	MMDBreaker::Cutter::getInstance().UpDate(victimModelData[num].trans, victimModelData[num].angle);
}

void Player::Draw() {

	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, skyDomeModelData.matrix);
	skyDome.Draw();
	if (isDraw) {
		DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD, victimModelData[num].matrix);
		victim[num].Draw();
		DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::WORLD,cutterModelData.matrix);
		cutter.Draw();
	}
	else {
		MMDBreaker::Cutter::getInstance().Draw();
	}
}

void Player::UnInit() {
	skyDome.UnInit();
	for (int i = 0; i < pmdModelSize; i++) {
		victim[i].UnInit();
	}
	cutter.UnInit();
	MMDBreaker::Cutter::getInstance().UnInit();
}
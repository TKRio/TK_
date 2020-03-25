#include "Scene.h"
#include "MouseInput.h"

using namespace DirectX;

bool Scene::Init(HWND hwnd) {
	cange = 0;
	//imgui�̏�����
	imgui.Init(hwnd, GetDX11Device(), GetDX11DeviceContext());

	//�^�C�g����ʂ̏�����
	//���C���摜
	std::string filename("texture/title.png");
	bool sts;
	sts = tex[0].Init(
		0,
		0,
		SCREEN_X,
		SCREEN_Y,
		XMFLOAT4(1.0f, 0.0f, 1.0f, 0.0f),		// ���_�J���[ 
		filename);

	if (!sts) {
		MessageBox(NULL, "load tex error", "error", MB_OK);
		return false;
	}
	//�I�����̉摜

	std::string filename2("texture/rogo.png");
	sts = tex[1].Init(
		100,
		100,
		200,
		200,
		XMFLOAT4(1.0f, 0.0f, 1.0f, 0.0f),		// ���_�J���[ 
		filename2);

	//�v���C���[�̏�����
	player.Init();
	return true;
}

void Scene::Updata() {
	switch (cange)
	{
	case 0:
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_SPACE)) {
			cange = 1;
			player.InitMat();
		}
		break;
	case 1:
		player.UpDate();
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_R)) {
			cange = 0;
		}
		break;
	default:
		break;
	}

}

void Scene::Draw() {
	switch (cange)
	{
	case 0:
		CDirectXGraphics::GetInstance()->TurnOffZbuffer();
		tex[0].ChangePosSize(title::posX,title::posY,title::scaleX,title::scaleY);
		tex[0].Draw();
		tex[1].ChangePosSize(rogo::posX,rogo::posY,rogo::scaleX, rogo::scaleY);
		tex[1].Draw();
		CDirectXGraphics::GetInstance()->TurnOnZBuffer();
		break;
	case 1:
		player.Draw();

		//imgui�̐ݒ�
		imgui.RenderStart();
		imgui.ParentWindow(&player);
		imgui.ChildrenWindow1(&player);
		imgui.RenderEnd();

		break;
	default:
		break;
	}
}

void Scene::Uninit() {
	for (int i = 0; i < texMax; i++) {
		tex[i].Uninit();
	}
	imgui.Uninit();
}
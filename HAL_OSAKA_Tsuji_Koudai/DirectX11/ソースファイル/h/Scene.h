#pragma once
#include "2DQuadtex.h"
#include "game.h"
#include "DX11util.h"
#include "Camera.h"
#include "imgui_app.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


const int texMax = 3;
const float mousePointerWidth = 10.0f;
const float mousePointerHight = 10.0f;

namespace title {
	const float posX = SCREEN_X / 2;
	const float posY = SCREEN_Y / 2;
	const float scaleX = SCREEN_X;
	const float scaleY = SCREEN_Y;
}

namespace rogo {
	const float posX = SCREEN_X / 2;
	const float posY = SCREEN_Y / 2.5f;
	const float scaleX = SCREEN_X;
	const float scaleY = SCREEN_Y / 3;
};


class Scene {
private:
	ImGuiAppFunc imgui;
	Player		 player;
	C2DQuadTex	 tex[texMax];
	POINT pt;

	int cange;

public:
	bool Init(HWND hwnd);
	void Updata();
	void Draw();
	void Uninit();
};
//*****************************************************************************
//!	@file	imgui_app.h
//!	@brief	imguiÇégópÇ∑ÇÈÇΩÇﬂÇÃä÷êîclass
//!	@note	
//!	@author	
//*****************************************************************************

#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Player.h"
//******************************************************************************
//class
//******************************************************************************
class ImGuiAppFunc {
private:
	float slider1;
public:
	void Init(HWND hwnd2, ID3D11Device* d11Dev, ID3D11DeviceContext* d11DevCon);
	void RenderStart();
	void ParentWindow(Player *p);
	void ChildrenWindow1(Player *p);
	void ChildrenWindow2();
	void RenderEnd();
	void Uninit();
};

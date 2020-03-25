#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_app.h"
#include <string>
#include <iostream>

using namespace DirectX;

void ImGuiAppFunc::Init(HWND hwnd2, ID3D11Device* d11Dev, ID3D11DeviceContext* d11DevCon) {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("assets/font/BIZ-UDGothicR.ttc",12.5f,NULL,io.Fonts->GetGlyphRangesJapanese());
	ImGui_ImplWin32_Init(hwnd2);
	ImGui_ImplDX11_Init(d11Dev, d11DevCon);

	ImGui::StyleColorsDark();
	slider1 = 1.0f;
}

void ImGuiAppFunc::RenderStart() {
	bool show_demo_window	 = true;
	bool show_another_window = false;
	static int counter = 0;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(50, 50));

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiAppFunc::ParentWindow(Player *p) {
	//プレイヤーの座標の表示
	ImGui::Begin("debug window Player");
	std::string str;
	str = "Player Position\nX: " + std::to_string(p->GetMatrix()._41)
		+ "\nY: " + std::to_string(p->GetMatrix()._42)
		+ "\nZ: " + std::to_string(p->GetMatrix()._43);
	ImGui::Text(str.c_str());

	static float trans[3] = { p->GetTrans().x,p->GetTrans().y,p->GetTrans().z };
	ImGui::InputFloat3("input Position",trans);
	if (ImGui::Button("decide")) {
		p->SetTrans(XMFLOAT3(trans[0],trans[1],trans[2]));
	}
	if (ImGui::Button("Resset Position")) {
		p->SetTrans(XMFLOAT3(0.0f,-0.0f,0.0f));
		for (int i = 0; i < 3; i++) {
			trans[i] = 0.0f;
		}
	}

	for (int i = 0; i < 3; i++) {
		ImGui::Spacing();
	}

	str = "Player Rotate\nX: " + std::to_string(p->GetAngle().x)
		+ "\nY: " + std::to_string(p->GetAngle().y)
		+ "\nZ: " + std::to_string(p->GetAngle().z);
	ImGui::Text(str.c_str());
	if (ImGui::Button("Resset angle")) {
		p->SetAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	}
	ImGui::Spacing();
	ImGui::Spacing();
}


void ImGuiAppFunc::ChildrenWindow1(Player *p) {
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Begin("debug window Camera");
	std::string str;

	str = "Camera Rotate\nX: " + std::to_string(p->GetCameraData().elevation)
		+ "\nY: " + std::to_string(p->GetCameraData().azimuth)
		+ "\nZ: " + std::to_string(p->GetCameraData().depth);
	ImGui::Text(str.c_str());

	static float rotate[3] = { p->GetCameraData().elevation,p->GetCameraData().azimuth,p->GetCameraData().depth };
	ImGui::InputFloat3("input Rotate", rotate);
	if (ImGui::Button("decide")) {
		p->SetCameraDataRotate(rotate[0], rotate[1], rotate[2]);
	}

	if (ImGui::Button("Resset angle")) {
		p->SetCameraDataRotate(-90.0f,90.0f,30.0f);
	}

	ImGui::Spacing();
	ImGui::Spacing();

	str = "Camera Position\nX: " + std::to_string(p->GetCameraData().lookat.x)
		+ "\nY: " + std::to_string(p->GetCameraData().lookat.y)
		+ "\nZ: " + std::to_string(p->GetCameraData().lookat.z);
	ImGui::Text(str.c_str());

	static float position[3] = { p->GetCameraData().lookat.x,p->GetCameraData().lookat.y,p->GetCameraData().lookat.z };
	ImGui::InputFloat3("input Position", position);
	if (ImGui::Button("decide")) {
		p->SetCameraDataPosition(position[0], position[1], position[2]);
	}

	if (ImGui::Button("Resset position")) {
		p->SetCameraDataPosition(0.0f, 10.0f, 0.0f);
	}

	ImGui::BulletText(u8"操作説明");
	str = u8"AWSD 　　　　　　 : Objctの移動\n"
		  u8"矢印キー　　　　  ： カメラ回転\n"
		  u8"Shift + 矢印キー  : Object回転\n"
		  u8"Enter 　　　　　  : MeshCut実行";
	ImGui::Text(str.c_str());
	ImGui::End();
}


void ImGuiAppFunc::ChildrenWindow2() {
	//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	//ImGui::Begin("debug window CollisionBullet");
	//std::string str;

	
	//ImGui::End();
}

void ImGuiAppFunc::RenderEnd() {
	// Rendering処理：一度だけ呼び出す
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiAppFunc::Uninit() {
	 ImGui_ImplDX11_Shutdown();
}
//*****************************************************************************
//!	@file	game.cpp
//!	@brief	
//!	@note	ゲーム処理
//!	@author	
//*****************************************************************************
#include "game.h"
#include "Scene.h"
using namespace DirectX;

//*****************************************************************************
// グロ－バル宣言
//*****************************************************************************
Scene scene;

//==============================================================================
//!	@fn		GameInit
//!	@brief	ゲーム初期処理
//!	@param	インスタンス値
//!	@param	ウインドウハンドル値
//!	@param	ウインドウサイズ幅（補正済みの値）
//!	@param	ウインドウサイズ高さ（補正済みの値）
//!	@param　	フルスクリーンフラグ　true ; フルスクリーン　false : ウインドウ
//!	@retval	true 成功　false 失敗
//==============================================================================
bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height, bool fullscreen)
{
	bool		sts;
	// DX11初期処理
	sts = DX11Init(hwnd, width, height, fullscreen);
	if (!sts) {
		MessageBox(hwnd, "DX11 init error", "error", MB_OK);
		return false;
	}

	//プロジェクション変換行列初期化(カメラ)
	XMFLOAT3 eye = {0.0f,10.0f,-18.0f};				// 視点
	XMFLOAT3 lookat = { 0.0f,10.0f,0.0f };			// 注視点
	XMFLOAT3 up = {0.0f,1.0f,0.0f};					// 上向きベクトル

	Camera::GetInstance()->Init(1.0f, 10000.0f, XM_PIDIV2, SCREEN_X, SCREEN_Y, eye, lookat, up);

	//定数バッファの初期化
	DX11SetTransform::GetInstance()->Init();

	// DIRECTINPUT初期化
	CDirectInput::GetInstance().Init(hinst, hwnd, width, height);

	// ライト初期化
	DX11LightInit(DirectX::XMFLOAT4(0,0,-1,0));		// 平行光源の方向をセット

	scene.Init(hwnd);

	return	true;
}

//==============================================================================
//!	@fn		GameInput
//!	@brief	ゲーム入力検知処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameInput(){
	CDirectInput::GetInstance().GetKeyBuffer();			// キー入力取得
	}

//==============================================================================
//!	@fn		GameUpdate
//!	@brief	ゲーム更新処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameUpdate(){
	scene.Updata();
}

//==============================================================================
//!	@fn		GameRender
//!	@brief	ゲーム更新処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameRender(){
	// ターゲットバッファクリア	
	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; //red,green,blue,alpha
	
	// デバイスコンテキスト取得
	ID3D11DeviceContext* devcontext;
	devcontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* rtv;
	rtv = CDirectXGraphics::GetInstance()->GetRenderTargetView();

	// ターゲットバッファクリア
	devcontext->ClearRenderTargetView(
		rtv,			// レンダーターゲットビュー
		ClearColor);	// クリアするカラー値

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* dstv;
	dstv = CDirectXGraphics::GetInstance()->GetDepthStencilView();

	// Zバッファ、ステンシルバッファクリア
	devcontext->ClearDepthStencilView(
		dstv,			// デプスステンシルビュー
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,			// Ｚバッファを1.0でクリアする(0.0f～1.0f)
		0);				// ステンシルバッファを0でクリアする


	XMFLOAT4X4 mtx;		// 行列

	// ビュー変換行列セット
	mtx = Camera::GetInstance()->GetCameraMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::VIEW, mtx);

	// プロジェクション変換行列セット
	mtx = Camera::GetInstance()->GetProjectionMatrix();
	DX11SetTransform::GetInstance()->SetTransform(DX11SetTransform::TYPE::PROJECTION, mtx);

	scene.Draw();

	// 描画後処理
	IDXGISwapChain* swapchain;
	swapchain = CDirectXGraphics::GetInstance()->GetSwapChain();
	swapchain->Present(
		0,		// フレームの表示を垂直方向の空白と同期させる方法を指定する整数。
		0);		// スワップチェーン表示オプションを含む整数値。 
				// これらのオプションはDXGI_PRESENT定数で定義されています。
	
}

//==============================================================================
//!	@fn		GameMain
//!	@brief	ゲームループ処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameMain()
{
	GameInput();					// 入力
	GameUpdate();					// 更新
	GameRender();					// 描画
}

//==============================================================================
//!	@fn		GameExit
//!	@brief	ゲーム終了処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameExit()
{
	scene.Uninit();
	DX11Uninit();
}

//******************************************************************************
//	End of file.
//******************************************************************************

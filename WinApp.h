#pragma once
#include "Windows.h"
#include <cstdint>

class WinApp
{
public:
	//メンバ変数	
	//ウィンドウサイズ
	static const int window_width = 1280;	//横幅
	static const int window_height = 720;	//立幅
	// ウィンドウクラス名
	static const wchar_t WindowClassName[];
	//ウィンドウプロシージャ
	LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	static WinApp* Initialize();
	//ウィンドウを生成
	void CreateGameWindow(
		const char* title = "DirectXGame", UINT windowStyle = WS_OVERLAPPEDWINDOW,
		int32_t clientWidth = window_width, int32_t clientHeight = window_height);
};


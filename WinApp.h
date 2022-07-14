#pragma once
#include "Windows.h"
#include "cstdint"

static const int window_width = 1280;	//横幅
static const int window_height = 720;	//立幅

class WinApp
{
public:
	//ウィンドウプロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	static WinApp* GetInstance();
	//ウィンドウ生成
	void CreateWindow_(const wchar_t* title);
	//ウィンドウクラスを登録解除
	void deleteWindow();

public:
	//メンバ変数
	WNDCLASSEX w{};
	HWND hwnd;
};
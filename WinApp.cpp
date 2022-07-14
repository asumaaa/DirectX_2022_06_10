#include "WinApp.h"
#include <string>

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 1;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WinApp* WinApp::GetInstance()
{
	static WinApp instance;
	return &instance;
}

void WinApp::CreateWindow_(const wchar_t* title)
{
	// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;	//ウィンドウプロシージャを設定
	w.lpszClassName = L"DirectXGame";		//ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr);	//ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);//カーソル指定

	//ウィンドウクラスをOSに登録する
	RegisterClassEx(&w);
	//ウィンドウサイズう
	RECT wrc = { 0,0,window_width,window_height };
	//自動でサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindow(
		w.lpszClassName,		//クラス名
		title,			//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//標準的なウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標
		CW_USEDEFAULT,			//表示Y座標
		wrc.right - wrc.left,	//ウィンドウ横幅
		wrc.bottom - wrc.top,	//ウィンドウ縦幅
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		w.hInstance,			//呼び出しアプリケーションハンドル
		nullptr					//オプション
	);

	//ウィンドウを表示状態にする
	ShowWindow(hwnd, SW_SHOW);
}

void WinApp::deleteWindow()
{
	UnregisterClass(w.lpszClassName, w.hInstance);
}

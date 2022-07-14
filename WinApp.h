#pragma once
#include "Windows.h"
#include "cstdint"

class WinApp
{
public:
	//ウィンドウサイズ
	static const int window_width = 1280;	//横幅
	static const int window_height = 720;	//立幅

	//名前
	static const wchar_t  windowClassName[];

	//ウィンドウのサイズを変えられるか
	enum class SizeChangeMode {
		None,        //!< サイズ変更不可
		Normal,      //!< 自由変更
		FixedAspect, //!< アスペクト比一定
	};

public:
	WinApp* GetInstance();

	//ウィンドウプロシージャ
	LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	//ウィンドウ生成
	void CreateGameWindow(
		const char* title = "DirectXGame", UINT windowStyle = WS_OVERLAPPEDWINDOW,
		int32_t clientWidth = window_width, int32_t clientHeight = window_height);

	//ウィンドウ破棄
	void TerminateGameWindow();

	//終了かどうか
	bool ProcessMessage();

	//ゲッター
	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return wndClass_.hInstance; }

	//フルスクリーン設定
	void SetFullscreen(bool fullscreen);
	//フルスクリーンかどうか
	bool IsFullscreen() const;
	//サイズ変更モードの設定
	void SetSizeChangeMode(SizeChangeMode sizeChangeMode);
	//サイズ変更モードかどうか
	SizeChangeMode GetSizeChangeMode() const;

private: // メンバ関数
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;

private: // メンバ変数
// Window関連
	HWND hwnd_ = nullptr;   // ウィンドウハンドル
	WNDCLASSEX wndClass_{}; // ウィンドウクラス
	UINT windowStyle_;
	bool isFullscreen_ = false;
	RECT windowRect_;
	SizeChangeMode sizeChangeMode_ = SizeChangeMode::Normal;
	float aspectRatio_;
};


#include "input.h"
#include <cassert>

Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}

void Input::Initialize(WinApp* winApp)
{
	HRESULT result;
	winApp_ = winApp;

	//キーボードの設定
	//DirectInputの初期化
	result = DirectInput8Create(
		winApp->w.hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput,
		nullptr
	);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);	//標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		winApp->hwnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	//キーボード情報の取得開始
	keyboard->Acquire();
	//全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(key), key);
}

#pragma once

#include <array>
#include <vector>
#include <wrl.h>

#include <XInput.h>
#include "WinApp.h"

#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
using namespace Microsoft::WRL;
#include <dinput.h>

class Input
{
public:
	static Input* GetInstance();
	void Initialize(WinApp* winApp);
	void Update();
public:
	//ウィンドウ
	WinApp* winApp_;

	ComPtr<IDirectInput8> directInput;
	//キーボード
	ComPtr<IDirectInputDevice8> keyboard;
	BYTE key[256] = {};
};


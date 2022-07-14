#pragma once
#include "WinApp.h"
#include "DirectXIni.h"
#include "string"
#include "DirectXMath.h"
#include "d3dcompiler.h"
#include "dinput.h"
#include "assert.h"
#include "DirectXTex.h"
#include "object3D.h"

#define DIRECTINPUT_VERSION	0x0800	//DirectInputtのバージョン指定

using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")

//DirectX初期化用の変数
HRESULT result;

//カメラの回転角
float angle = 0.0f;

//座標
XMFLOAT3 position = { 0.0f,0.0f,0.0f };
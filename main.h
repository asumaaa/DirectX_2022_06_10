#pragma once
#include "WinApp.h"
#include "DirectXIni.h"
#include "input.h"
#include "Masage.h"
#include "Texture.h"
#include "VertBuff.h"
#include "IndexBuff.h"
#include "Shader.h"
#include "Pipe.h"
#include "Depth.h"
#include "RootSig.h"
#include "Square2.h"
#include "Triangle.h"

#include "string"
#include "DirectXMath.h"
//#include "d3dcompiler.h"
#include "dinput.h"
#include "assert.h"
#include "DirectXTex.h"
#include "object3D.h"

using namespace DirectX;
using namespace Microsoft::WRL;

//#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")

//DirectX‰Šú‰»—p‚Ì•Ï”
HRESULT result;

//ƒJƒƒ‰‚Ì‰ñ“]Šp
float angle = 0.0f;

//À•W
XMFLOAT3 position = { 0.0f,0.0f,0.0f };
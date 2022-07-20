#pragma once
#include "WinApp.h"
#include "DirectXIni.h"
#include "input.h"
#include "Masage.h"
#include "Texture.h"
#include "VertBuff.h"
#include "IndexBuff.h"
#include "Shader.h"

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

class Pipe
{
public:
	static Pipe GetInstance();
	void Initialize();
	void Update();
public:

};


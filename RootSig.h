#pragma once
#include "DirectXIni.h"
#include "Shader.h"
#include "assert.h"
#include "DirectXTex.h"

using namespace DirectX;
using namespace Microsoft::WRL;

//#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")

class RootSig
{
public:
	static RootSig* GetInstance();
	void Initialize(Shader shader_,DirectXIni* dx_);
public:
	ComPtr<ID3D12RootSignature> rootSignature;
	DirectXIni* dx;
	Shader shader;
};


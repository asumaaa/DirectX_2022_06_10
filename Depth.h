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

class Depth
{
public:
	static Depth* GetInstance();
	void Initialize(DirectXIni* dx_);
	void Update(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle);
	void Update2();
public:
	DirectXIni* dx;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
};


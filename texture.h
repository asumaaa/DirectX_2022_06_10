#pragma once

#pragma once
#include "Windows.h"
#include "d3d12.h"
#include "dxgi1_6.h"
#include "cassert"
#include "vector"
#include "string"
#include "DirectXMath.h"
#include "d3dcompiler.h"
#include "dinput.h"
#include "assert.h"
#include "DirectXTex.h"

#define DIRECTINPUT_VERSION	0x0800	//DirectInputtのバージョン指定

using namespace DirectX;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


//頂点データ構造体
struct Vertex
{
	XMFLOAT3 pos;	//座標
	XMFLOAT2 uv;	//uv座標
};

class texture
{
private:
	Vertex vertices[4];
	HRESULT result;
	ID3D12Device* device = nullptr;
public:
	void Initialize(Vertex vertices_[4],const wchar_t graph, ID3D12Device* device_);
	void Draw();
};


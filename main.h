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
#include "Triangle.h"
#include "Vector.h"

#define DIRECTINPUT_VERSION	0x0800	//DirectInputtのバージョン指定

using namespace DirectX;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//ウィンドウサイズ
const int window_width = 1280;	//横幅
const int window_height = 720;	//立幅

//DirectX初期化用の変数
HRESULT result;
ID3D12Device* device = nullptr;
IDXGIFactory7* dxgiFactory = nullptr;
IDXGISwapChain4* swapChain = nullptr;
ID3D12CommandAllocator* commandAllocator = nullptr;
ID3D12GraphicsCommandList* commandList = nullptr;
ID3D12CommandQueue* commandQueue = nullptr;
ID3D12DescriptorHeap* rtvHeap = nullptr;
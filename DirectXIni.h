#pragma once
#include <Windows.h>
#include <cstdlib>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <algorithm>
#include <cassert>
#include <vector>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include "WinApp.h"

using namespace Microsoft::WRL;

class DirectXIni
{
public:
	static DirectXIni* GetInstance();
	void Initialize(WinApp* winApp);
public:
	//メンバ変数
	//ウィンドウ
	WinApp* winApp_;
	//DirectX
	ComPtr<ID3D12Debug> debugController;
	ComPtr<IDXGIFactory7> dxgiFactory;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;

	ComPtr<ID3D12Fence> fence;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	std::vector<ComPtr<ID3D12Resource>>backBuffers;
	std::vector<ComPtr<IDXGIAdapter4>>adapters;
	ComPtr<IDXGIAdapter4> tmpAdapter;

	UINT64 fenceVal = 0;
};


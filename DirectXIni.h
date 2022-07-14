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
	//ゲッター
	ID3D12Device* GetDevice() { return device.Get(); }
	IDXGISwapChain4* GetSwapChain() { return swapChain.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList.Get(); }
	ID3D12Debug* GetDebugController() { return debugController.Get(); }
	IDXGIFactory7* GetDxgiFactory() { return dxgiFactory.Get(); }
	ID3D12CommandAllocator* GetCommandAllocator() { return commandAllocator.Get(); }
	ID3D12CommandQueue* GetCommandQueue() { return commandQueue.Get(); }
	ID3D12DescriptorHeap* GetRtvHeap() { return rtvHeap.Get(); }
	ID3D12Fence* GetFence() { return fence.Get(); }
public:
	//メンバ変数
	//ウィンドウ
	WinApp* winApp_;
private:
	//DirectX
	ComPtr<ID3D12Device> device;
	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12Debug> debugController;
	ComPtr<IDXGIFactory7> dxgiFactory;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Fence> fence;

public:
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	std::vector<ComPtr<ID3D12Resource>>backBuffers;
	std::vector<ComPtr<IDXGIAdapter4>>adapters;
	ComPtr<IDXGIAdapter4> tmpAdapter;

	UINT64 fenceVal = 0;
};


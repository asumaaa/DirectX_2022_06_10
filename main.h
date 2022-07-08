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
#include "wrl.h"
#include "object3D.h"

#define DIRECTINPUT_VERSION	0x0800	//DirectInputt�̃o�[�W�����w��

using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//�E�B���h�E�T�C�Y
const int window_width = 1280;	//����
const int window_height = 720;	//����

//DirectX�������p�̕ϐ�
HRESULT result;
ComPtr<ID3D12Device> device;
ComPtr<IDXGIFactory7> dxgiFactory;
ComPtr<IDXGISwapChain4> swapChain;
ComPtr<ID3D12CommandAllocator> commandAllocator;
ComPtr<ID3D12GraphicsCommandList> commandList;
ComPtr<ID3D12CommandQueue> commandQueue;
ComPtr<ID3D12DescriptorHeap> rtvHeap;

//�J�����̉�]�p
float angle = 0.0f;

//���W
XMFLOAT3 position = { 0.0f,0.0f,0.0f };
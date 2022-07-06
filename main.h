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

#define DIRECTINPUT_VERSION	0x0800	//DirectInputtのバージョン指定

using namespace DirectX;
using namespace Microsoft::WRL;

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
ComPtr<ID3D12Device> device;
ComPtr<IDXGIFactory7> dxgiFactory;
ComPtr<IDXGISwapChain4> swapChain;
ComPtr<ID3D12CommandAllocator> commandAllocator;
ComPtr<ID3D12GraphicsCommandList> commandList;
ComPtr<ID3D12CommandQueue> commandQueue;
ComPtr<ID3D12DescriptorHeap> rtvHeap;

//定数バッファ用データ構造体(マテリアル)
struct ConstBufferDataMaterial {
	XMFLOAT4 color;	//色(RGBA)
};

//定数バッファ用データ構造体（3D変換行列）
struct ConstBufferDataTransform
{
	XMMATRIX mat;
};

//頂点データ構造体
struct Vertex
{
	XMFLOAT3 pos;	//座標
	XMFLOAT3 normalize;	//法線ベクトル
	XMFLOAT2 uv;	//uv座標
};

//カメラの回転角
float angle = 0.0f;

//座標
XMFLOAT3 position = { 0.0f,0.0f,0.0f };
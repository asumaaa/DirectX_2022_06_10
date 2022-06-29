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

#define DIRECTINPUT_VERSION	0x0800	//DirectInputt�̃o�[�W�����w��

using namespace DirectX;

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
ID3D12Device* device = nullptr;
IDXGIFactory7* dxgiFactory = nullptr;
IDXGISwapChain4* swapChain = nullptr;
ID3D12CommandAllocator* commandAllocator = nullptr;
ID3D12GraphicsCommandList* commandList = nullptr;
ID3D12CommandQueue* commandQueue = nullptr;
ID3D12DescriptorHeap* rtvHeap = nullptr;

//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
struct ConstBufferDataMaterial {
	XMFLOAT4 color;	//�F(RGBA)
};

//�萔�o�b�t�@�p�f�[�^�\���́i3D�ϊ��s��j
struct ConstBufferDataTransform
{
	XMMATRIX mat;
};

//���_�f�[�^�\����
struct Vertex
{
	XMFLOAT3 pos;	//���W
	XMFLOAT2 uv;	//uv���W
};

//�J�����̉�]�p
float angle = 0.0f;
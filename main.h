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
	XMFLOAT3 normalize;	//�@���x�N�g��
	XMFLOAT2 uv;	//uv���W
};

//�J�����̉�]�p
float angle = 0.0f;

//���W
XMFLOAT3 position = { 0.0f,0.0f,0.0f };
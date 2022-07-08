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

//3D�I�u�W�F�N�g�^
struct Object3d
{
	//�萔�o�b�t�@
	ID3D12Resource* constBuffTransform;
	//�萔�o�b�t�@�}�b�v
	ConstBufferDataTransform* constMapTransform;
	//�A�t�B���ϊ����
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	//���[���h�ϊ��s��
	XMMATRIX matWorld;
	//�e�I�u�W�F�N�g�̃|�C���^�[
	Object3d* parent = nullptr;
};

//3�I�u�W�F�N�g�֘A
//3D�I�u�W�F�N�g������
void InitializeObject3d(Object3d* object, ComPtr<ID3D12Device> device);
//3D�I�u�W�F�N�g�X�V
void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection);
//�I�u�W�F�N�g�`�揈��
void DrawObject3d(Object3d* object, ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

//�摜�f�[�^�\����
struct texData
{
	//�摜
	TexMetadata metadata;
	ScratchImage scratchImg;
	//�~�b�v�}�b�v
	ScratchImage mipChain;
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff;
	//�e�N�X�`���[��GPU�̃n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	//�摜�p�f�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* srvHeap = nullptr;
};

//�摜�f�[�^������
void InitializeTexData(texData* tex, const wchar_t* szFile, ComPtr<ID3D12Device> device, int texNum);
//�摜�f�[�^�`��
void DrawTex3d(texData* tex, ComPtr<ID3D12GraphicsCommandList> commandList);
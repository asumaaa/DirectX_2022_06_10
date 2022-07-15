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

#include "DirectXIni.h"

#define DIRECTINPUT_VERSION	0x0800	//DirectInputt�̃o�[�W�����w��

using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//���_�f�[�^�\����
struct Vertex
{
	XMFLOAT3 pos;	//���W
	XMFLOAT3 normalize;	//�@���x�N�g��
	XMFLOAT2 uv;	//uv���W
};

//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
struct ConstBufferDataMaterial {
	XMFLOAT4 color;	//�F(RGBA)
};

//�萔�o�b�t�@�p�f�[�^�\���́i3D�ϊ��s��j
struct ConstBufferDataTransform
{
	XMMATRIX mat;
};

class Square
{
public:
	Square(XMFLOAT3 size);
	void Initialize(DirectXIni * dx);
	void Update();
//public:
//	//�Q�b�^�[
//	D3D12_HEAP_PROPERTIES GetHeapProp() { return heapProp; }
//	D3D12_VERTEX_BUFFER_VIEW GetVbView() { return vbView; }	
//	D3D12_INDEX_BUFFER_VIEW GetIbView() { return ibView; }	
//	ID3DBlob* GetVsBlob() { return vsBlob.Get(); }	
//	ID3DBlob* GetPsBlob() { return psBlob.Get(); }	
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetPipelineDesc() { return pipelineDesc; }
//	D3D12_RENDER_TARGET_BLEND_DESC GetBlenddesc() { return blenddesc; }
	//ID3D12DescriptorHeap* GetDsvHeap() { return dsvHeap.Get(); }
//	ID3D12RootSignature* GetRootSignature() { return rootSignature.Get(); }
//	ID3D12PipelineState* GetPipelineState() { return pipelineState.Get(); }
//	ID3D12Resource* GetConstBuffMaterial() { return constBuffMaterial.Get(); }
public:
	//���_�f�[�^
	Vertex vertices[24];
	//�C���f�b�N�X�f�[�^
	unsigned short indices[24];
	//���_�f�[�^�S�̂̃T�C�Y
	UINT sizeVB;
	//�C���f�b�N�X�o�b�t�@�S�̂̃T�C�Y
	UINT sizeIB;
	DirectXIni* dx_;

public:
	//�o�b�t�@�Ƃ�
	D3D12_HEAP_PROPERTIES heapProp{};	//�@���_�o�b�t�@ �q�[�v�ݒ�
	D3D12_VERTEX_BUFFER_VIEW vbView{};	//���_�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView{};	//�C���f�b�N�X�o�b�t�@�r���[
	ComPtr<ID3DBlob> vsBlob;	//���_�V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};	//�O���t�B�b�N�X�p�C�v���C��
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�@�u�����h���ׂ����ݒ�ł���悤�ɂ���
	ComPtr<ID3D12DescriptorHeap> dsvHeap; //�[�x�X�e���V���r���[
	ComPtr<ID3D12RootSignature> rootSignature;	//���[�g�V�O�l�`��
	ComPtr<ID3D12PipelineState> pipelineState;	//�p�C�v���C���X�e�[�g
	ComPtr<ID3D12Resource> constBuffMaterial;	//�萔�o�b�t�@
};


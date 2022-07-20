#pragma once
#include "Windows.h"
#include "d3d12.h"
#include "dxgi1_6.h"
#include "cassert"
#include "vector"
#include "string"
#include "DirectXMath.h"
#include "assert.h"
#include "DirectXTex.h"
#include "wrl.h"

using namespace DirectX;
using namespace Microsoft::WRL;

//定数バッファ用データ構造体(マテリアル)
struct ConstBufferDataMaterial {
	XMFLOAT4 color;	//色(RGBA)
};

//定数バッファ用データ構造体（3D変換行列）
struct ConstBufferDataTransform
{
	XMMATRIX mat;
};

//3Dオブジェクト型
struct Object3d
{
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform;
	//定数バッファマップ
	ConstBufferDataTransform* constMapTransform;
	//アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matWorld;
	//親オブジェクトのポインター
	Object3d* parent = nullptr;
};

//3オブジェクト関連
//3Dオブジェクト初期化
void InitializeObject3d(Object3d* object, ComPtr<ID3D12Device> device);
//3Dオブジェクト更新
void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection);
//オブジェクト描画処理
void DrawObject3d(Object3d* object, ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

//画像データ構造体
struct texData
{
	//画像
	TexMetadata metadata;
	ScratchImage scratchImg;
	//ミップマップ
	ScratchImage mipChain;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff;
	//テクスチャーのGPUのハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	//画像用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap;
};

//画像データ初期化
void InitializeTexData(texData* tex,const wchar_t* szFile, ComPtr<ID3D12Device> device, int texNum);
//画像データ描画
void DrawTex3d(texData* tex,ComPtr<ID3D12GraphicsCommandList> commandList);
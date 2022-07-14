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

#include "DirectXIni.h"

using namespace DirectX;
using namespace Microsoft::WRL;

////画像データ構造体
//struct texData
//{
//	//画像
//	TexMetadata metadata;
//	ScratchImage scratchImg;
//	//ミップマップ
//	ScratchImage mipChain;
//	//テクスチャバッファ
//	ComPtr<ID3D12Resource> texBuff;
//	//テクスチャーのGPUのハンドル
//	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
//	//画像用デスクリプタヒープ
//	ComPtr<ID3D12DescriptorHeap> srvHeap;
//};

class Texture
{
public:
	static Texture* GetInstance();
	void Initialize(const wchar_t* szFile,DirectXIni *dx,int texNum);
	void Draw();

	//ゲッター
	ID3D12Resource* GetTexBuff() { return texBuff.Get(); }
	ID3D12DescriptorHeap* GetSrvHeap() { return srvHeap.Get(); }
private:
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
public:
	DirectXIni* dx_ ;
};


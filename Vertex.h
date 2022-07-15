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

#define DIRECTINPUT_VERSION	0x0800	//DirectInputtのバージョン指定

using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//頂点データ構造体
struct Vertex
{
	XMFLOAT3 pos;	//座標
	XMFLOAT3 normalize;	//法線ベクトル
	XMFLOAT2 uv;	//uv座標
};

//定数バッファ用データ構造体(マテリアル)
struct ConstBufferDataMaterial {
	XMFLOAT4 color;	//色(RGBA)
};

//定数バッファ用データ構造体（3D変換行列）
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
//	//ゲッター
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
	//頂点データ
	Vertex vertices[24];
	//インデックスデータ
	unsigned short indices[24];
	//頂点データ全体のサイズ
	UINT sizeVB;
	//インデックスバッファ全体のサイズ
	UINT sizeIB;
	DirectXIni* dx_;

public:
	//バッファとか
	D3D12_HEAP_PROPERTIES heapProp{};	//　頂点バッファ ヒープ設定
	D3D12_VERTEX_BUFFER_VIEW vbView{};	//頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};	//インデックスバッファビュー
	ComPtr<ID3DBlob> vsBlob;	//頂点シェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob;	//ピクセルシェーダーオブジェクト
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};	//グラフィックスパイプライン
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];	//レンダーターゲットのブレンド設定　ブレンドを細かく設定できるようにする
	ComPtr<ID3D12DescriptorHeap> dsvHeap; //深度ステンシルビュー
	ComPtr<ID3D12RootSignature> rootSignature;	//ルートシグネチャ
	ComPtr<ID3D12PipelineState> pipelineState;	//パイプラインステート
	ComPtr<ID3D12Resource> constBuffMaterial;	//定数バッファ
};


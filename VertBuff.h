#pragma once
#include "Vertex.h"
#include "DirectXIni.h"

class VertBuff
{
public:
	static VertBuff* GetInstance();
	void Initialize(Ver* vertex, DirectXIni* dx_);
	void Initialize(Ver2 *vertex,DirectXIni* dx_);
	D3D12_VERTEX_BUFFER_VIEW *GetVbView() { return &vbView; }
public:
	ComPtr<ID3D12Resource> vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView{};		//頂点バッファビュー
};


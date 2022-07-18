#pragma once
#include "Vertex.h"
#include "DirectXIni.h"

class VertBuff
{
public:
	VertBuff(Ver *vertex,DirectXIni* dx_);
	D3D12_VERTEX_BUFFER_VIEW *GetVbView() { return &vbView; }
public:
	ComPtr<ID3D12Resource> vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView{};		//頂点バッファビュー
};


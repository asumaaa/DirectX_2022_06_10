#pragma once
#include "Vertex.h"
#include "DirectXIni.h"

class IndexBuff
{
public:
	static IndexBuff* GetInstance();
	void Initialize(Ver* vertex, DirectXIni* dx_);
	void Initialize(Ver2* vertex, DirectXIni* dx_);
	D3D12_INDEX_BUFFER_VIEW* GetIbView() { return &ibView; }
public:
	ComPtr<ID3D12Resource> indexBuff;
	D3D12_INDEX_BUFFER_VIEW ibView{};
};


#pragma once
#include "DirectXIni.h"
#include "Vertex.h"
#include "IndexBuff.h"
#include "VertBuff.h"
#include "Shader.h"
#include "RootSig.h"
#include "Pipe.h"
#include "d3d12.h"

class Triangle
{
public:
	static Triangle* GetInstance();
	void Initialize(XMFLOAT3 size, DirectXIni* dx_,const wchar_t* file);
	void Update();
public:
	Ver2* vertex = nullptr;
	IndexBuff indexBuff;
	VertBuff vertBuff;
	Shader shader;
	RootSig rootSig;
	Pipe pipe;
	DirectXIni* dx;
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12Resource> constBuffMaterial;
public:
	D3D12_RECT scissorRect{};
	D3D12_VIEWPORT viewport{};
};


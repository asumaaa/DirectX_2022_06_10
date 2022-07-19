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

#include "VertBuff.h"
#include "IndexBuff.h"
#include "Shader.h"
#include "Vertex.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Pipeline
{
public:
	void Initialize(Shader shader,Ver* vertex);
public:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc{};//グラフィックスパイプラインの各ステージの設定をする構造体
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc;//レンダーターゲットのブレンド設定
};


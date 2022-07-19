#include "Pipeline.h"

void Pipeline::Initialize(Shader shader,Ver *vertex)
{

	//シェーダをパイプラインに設定
	Desc.VS.pShaderBytecode = shader.vsBlob->GetBufferPointer();
	Desc.VS.BytecodeLength = shader.vsBlob->GetBufferSize();
	Desc.PS.pShaderBytecode = shader.psBlob->GetBufferPointer();
	Desc.PS.BytecodeLength = shader.psBlob->GetBufferSize();

	//サンプルマスクの設定
	Desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//標準設定

	//ラスタライザの設定
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//カリングしない
	Desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;	//背面をカリング
	Desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//ポリゴン塗りつぶし
	Desc.RasterizerState.DepthClipEnable = true;	//深度クリッピングを有効に

	//ブレンドステートを有効に
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask =
	//	D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定　ブレンドを細かく設定できるようにする
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = Desc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBA全てのチャンネルを描画

	//ブレンド設定
	//共通設定(アルファ値)
	blenddesc.BlendEnable = true;					//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//デストの値を0  %使う
	////加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;	//加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;		//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;	//デストの値を100%使う
	////減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;	//デストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;				//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;				//デストの値を100%使う
	////色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	//1.0f-デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;				//使わない
	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//頂点レイアウトの設定
	Desc.InputLayout.pInputElementDescs = vertex->inputLayout;
	Desc.InputLayout.NumElements = _countof(vertex->inputLayout);

	//図形の形状設定
	Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	Desc.NumRenderTargets = 1;	//描画対象の数
	Desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0~255指定のRGBA
	Desc.SampleDesc.Count = 1;	//1ピクセルにつき1回サンプリング
}

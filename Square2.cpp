#include "Square2.h"

Square2* Square2::GetInstance()
{
	static Square2 instance;
	return &instance;
}

void Square2::Initialize(XMFLOAT3 size, DirectXIni* dx_)
{
	dx = dx_;
	HRESULT result;

	vertex = Ver::GetInstance();
	vertex->Initialize(size);
	indexBuff.GetInstance();
	indexBuff.Initialize(vertex, dx);
	vertBuff.GetInstance();
	vertBuff.Initialize(vertex, dx);
	shader.GetInstance();
	shader.compileVs(L"BasicVS.hlsl");
	shader.compilePs(L"BasicPS.hlsl");
	rootSig.GetInstance();
	rootSig.Initialize(shader, dx);
	pipe.GetInstance();
	pipe.Initialize(shader, rootSig, vertex, dx);

	//パイプラインステート
	result = dx->GetDevice()->CreateGraphicsPipelineState(&pipe.pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));


	//定数バッファ
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;	//256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//定数バッファの生成
	result = dx->GetDevice()->CreateCommittedResource(
		&cbHeapProp,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,	//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial)
	);
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//マッピング
	assert(SUCCEEDED(result));
	//値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Square2::Update()
{
	//ビューポート設定コマンド
	viewport.Width = window_width;
	viewport.Height = window_height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//ビューポート設定コマンドをコマンドリストに積む
	dx->GetCommandList()->RSSetViewports(1, &viewport);

	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + window_width;
	scissorRect.top = 0;
	scissorRect.bottom = scissorRect.top + window_height;
	//シザー矩形設定コマンドをコマンドリストに積む
	dx->GetCommandList()->RSSetScissorRects(1, &scissorRect);
	//パイプラインステートをセット
	dx->GetCommandList()->SetPipelineState(pipelineState.Get());
	//ルートシグネチャをセット
	dx->GetCommandList()->SetGraphicsRootSignature(rootSig.rootSignature.Get());
	//プリミティブ形状の設定コマンド
	dx->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//三角形リスト
	//定数バッファビュー(CBV)の設定コマンド
	dx->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
}

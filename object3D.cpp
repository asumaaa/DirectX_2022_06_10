#include "object3D.h"

//3Dオブジェクト初期化
void InitializeObject3d(Object3d* object, ComPtr<ID3D12Device> device)
{
	HRESULT result;

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;	//256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffTransform)
	);
	assert(SUCCEEDED(result));
	//定数バッファのマッピング
	result = object->constBuffTransform->Map(0, nullptr, (void**)&object->constMapTransform);
	assert(SUCCEEDED(result));
}

void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection)
{
	XMMATRIX matScale, matRot, matTrans;

	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation.z);
	matRot *= XMMatrixRotationX(object->rotation.x);
	matRot *= XMMatrixRotationY(object->rotation.y);

	matTrans = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);

	//ワールド行列の合成
	object->matWorld = XMMatrixIdentity();
	object->matWorld *= matScale;
	object->matWorld *= matRot;
	object->matWorld *= matTrans;

	//親オブジェクトがあれば
	if (object->parent != nullptr)
	{
		object->matWorld *= object->parent->matWorld;
	}

	//定数バッファへデータ転送
	object->constMapTransform->mat = object->matWorld * matView * matProjection;
}

//オブジェクト描画処理
void DrawObject3d(Object3d* object, ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices)
{
	//頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &vbView);
	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());
	//インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&ibView);
	//描画コマンド
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);	//全ての頂点を使って描画
}

void InitializeTexData(texData* tex, const wchar_t* szFile, ComPtr<ID3D12Device> device,  int texNum)
{
	HRESULT result;
	UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);	//デスクリプタのサイズ

	result = LoadFromWICFile(
		szFile,
		WIC_FLAGS_NONE,
		&tex->metadata, tex->scratchImg
	);


	result = GenerateMipMaps(
		tex->scratchImg.GetImages(),
		tex->scratchImg.GetImageCount(),
		tex->scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT,
		0,
		tex->mipChain
	);
	if (SUCCEEDED(result))
	{
		tex->scratchImg = std::move(tex->mipChain);
		tex->metadata = tex->scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	tex->metadata.format = MakeSRGB(tex->metadata.format);

	//テクスチャバッファ設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = tex->metadata.format;
	textureResourceDesc.Width = tex->metadata.width;	//幅
	textureResourceDesc.Height = tex->metadata.height;	//高さ
	textureResourceDesc.DepthOrArraySize = (UINT16)tex->metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)tex->metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//テクスチャバッファの生成
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&tex->texBuff)
	);

	//全ミップマップについて
	for (size_t i = 0; i < tex->metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = tex->scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータを転送
		result = tex->texBuff->WriteToSubresource(
			/*(UINT)*/i,
			nullptr,				//全領域へコピー
			img->pixels,			//元データアドレス
			/*(UINT)*/img->rowPitch,	//1ラインサイズ
			/*(UINT)*/img->slicePitch	//1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	//い
	//デスクリプタヒープ生成
	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//設定をもとにSRV用デスクリプタヒープを生成
	result = device->CreateDescriptorHeap(
		&srvHeapDesc,
		IID_PPV_ARGS(&tex->srvHeap)
	);
	assert(SUCCEEDED(result));

	//SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = tex->srvHeap->GetCPUDescriptorHandleForHeapStart();
	//ここまで

	//テクスチャーの番号が0以降の場合ハンドルを進める
	if (texNum > 0)
	{
		srvHandle.ptr += (incrementSize * texNum);
	}
	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;
	//ハンドルの指す位置にシェーダリソースビュー作成
	device->CreateShaderResourceView(tex->texBuff.Get(), &srvDesc, srvHandle);

	//ハンドルの値を設定
	tex->srvGpuHandle = tex->srvHeap->GetGPUDescriptorHandleForHeapStart();
	//保存用変数
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle2 = tex->srvHeap->GetGPUDescriptorHandleForHeapStart();
	//テクスチャの番号が0以外の時srvGpuHandleの値を変更
	if (texNum > 0)
	{
		tex->srvGpuHandle.ptr = srvGpuHandle2.ptr + (incrementSize * texNum);
	}
}

void DrawTex3d(texData* tex, ComPtr<ID3D12GraphicsCommandList> commandList)
{
	ID3D12DescriptorHeap* ppHeaps[] = { tex->srvHeap.Get() };
	commandList->SetDescriptorHeaps(1, ppHeaps);
	//描画コマンド
	commandList->SetGraphicsRootDescriptorTable(1, tex->srvGpuHandle);
}
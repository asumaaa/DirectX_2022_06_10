#include "Texture.h"

Texture* Texture::GetInstance()
{
	static Texture instance;
	return &instance;
}

void Texture::Initialize(const wchar_t* szFile, DirectXIni* dx, int texNum)
{
    this->dx_ = dx;

	HRESULT result;
	UINT incrementSize = dx_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);	//デスクリプタのサイズ

	result = LoadFromWICFile(
		szFile,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);


	result = GenerateMipMaps(
		scratchImg.GetImages(),
		scratchImg.GetImageCount(),
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT,
		0,
		mipChain
	);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

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
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;	//幅
	textureResourceDesc.Height = metadata.height;	//高さ
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//テクスチャバッファの生成
	result = dx_->GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	//全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータを転送
		result = texBuff->WriteToSubresource(
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
	result = dx_->GetDevice()->CreateDescriptorHeap(
		&srvHeapDesc,
		IID_PPV_ARGS(&srvHeap)
	);
	assert(SUCCEEDED(result));

	//SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
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
	dx_->GetDevice()->CreateShaderResourceView(texBuff.Get(), &srvDesc, srvHandle);

	//ハンドルの値を設定
	srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	//保存用変数
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle2 = srvHeap->GetGPUDescriptorHandleForHeapStart();
	//テクスチャの番号が0以外の時srvGpuHandleの値を変更
	if (texNum > 0)
	{
		srvGpuHandle.ptr = srvGpuHandle2.ptr + (incrementSize * texNum);
	}
}

void Texture::Draw()
{
	ID3D12DescriptorHeap* ppHeaps[] = {srvHeap.Get() };
	dx_->GetCommandList()->SetDescriptorHeaps(1, ppHeaps);
	//描画コマンド
	dx_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
}

#include "main.h"


//ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

#pragma region ウィンドウ生成
	//ウィンドウクラスの生成
	WNDCLASSEX w{};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;	//ウィンドウプロシージャを設定
	w.lpszClassName = L"DirectXGame";		//ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr);	//ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);//カーソル指定

	//ウィンドウクラスをOSに登録する
	RegisterClassEx(&w);
	//ウィンドウサイズう
	RECT wrc = { 0,0,window_width,window_height };
	//自動でサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(
		w.lpszClassName,		//クラス名
		L"DirectXGame",			//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//標準的なウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標
		CW_USEDEFAULT,			//表示Y座標
		wrc.right - wrc.left,	//ウィンドウ横幅
		wrc.bottom - wrc.top,	//ウィンドウ縦幅
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		w.hInstance,			//呼び出しアプリケーションハンドル
		nullptr					//オプション
	);

	//ウィンドウを表示状態にする
	ShowWindow(hwnd, SW_SHOW);
#pragma endregion

	MSG msg{};	//メッセージ

#pragma region DirectX初期化処理

#ifdef _DEBUG
	//デバッグレイヤーをオンに
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}

#endif

	//DXGIファクトリーの生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));

	//アダプターの列挙用
	std::vector<ComPtr<IDXGIAdapter4>>adapters;
	//ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter4> tmpAdapter;

	//パフォーマンスが高いものから順に、すべてのアダプターを列挙する
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(
			i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		//動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

	//妥当なアダプターを選別する
	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		//アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//デバイスを採用してループを抜ける
			tmpAdapter = adapters[i];
			break;
		}
	}

	//対応レベルの生成
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++)
	{
		//採用したアダプターをデバイスで生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device));
		if (result == S_OK)
		{
			//デバイス生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

	//コマンドアロケータを生成
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(result));

	//コマンドリストを生成
	result = device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));

	//コマンドキューに設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューを生成
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));

	//スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = 1280;
	swapChainDesc.Height = 720;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//色情報の書式
	swapChainDesc.SampleDesc.Count = 1;					//マルチサンプリングしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;	//バックバッファ用
	swapChainDesc.BufferCount = 2;						//バッファ数を2つに設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//フリップ後は破棄
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain1>swapchain1;

	//スワップチェーンの生成 
	result = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapchain1);

	swapchain1.As(&swapChain);
	assert(SUCCEEDED(result));

	// デスクリプタヒープの設定 
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー 
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount; //裏表の二つ

	// デスクリプタヒープの生成 
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

	//バックバッファ
	std::vector<ComPtr<ID3D12Resource>>backBuffers;
	backBuffers.resize(swapChainDesc.BufferCount);

	//スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers.size(); i++)
	{
		//スワップチェーンからバッファを取得
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		//デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		//裏か表でアドレスがずれる
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダーターゲットビュの生成
		device->CreateRenderTargetView(backBuffers[i].Get(), &rtvDesc, rtvHandle);
	}

	//フェンスの生成
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;
	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//キーボードの設定
	//DirectInputの初期化
	IDirectInput8* directInput = nullptr;
	result = DirectInput8Create(
		w.hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput,
		nullptr
	);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	IDirectInputDevice8* keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);	//標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		hwnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));

#pragma endregion

#pragma region 描画処理初期化

	//頂点データ
	Vertex vertices[] = {
		//前
		{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} },	//0
		{{-5.0f, 5.0f,-5.0f},{},{0.0f,0.0f} },	//1 
		{{ 5.0f,-5.0f,-5.0f},{},{1.0f,1.0f} },	//2 
		{{ 5.0f, 5.0f,-5.0f},{},{1.0f,0.0f} },	//3
		//後
		{{ 5.0f,-5.0f, 5.0f},{},{0.0f,1.0f} },	//4
		{{ 5.0f, 5.0f, 5.0f},{},{0.0f,0.0f} },	//5
		{{-5.0f,-5.0f, 5.0f},{},{1.0f,1.0f} },	//6
		{{-5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },	//7
		//左
		{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} },	//8
		{{-5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} },	//9
		{{-5.0f, 5.0f,-5.0f},{},{1.0f,1.0f} },	//10
		{{-5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },	//11
		//右
		{{ 5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} },	//12
		{{ 5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} },	//13
		{{ 5.0f, 5.0f,-5.0f},{},{1.0f,1.0f} },	//14
		{{ 5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },	//15
		//下
		{{-5.0f,-5.0f, 5.0f},{},{0.0f,1.0f} },	//16
		{{-5.0f,-5.0f,-5.0f},{},{0.0f,0.0f} },	//17
		{{ 5.0f,-5.0f, 5.0f},{},{1.0f,1.0f} },	//18
		{{ 5.0f,-5.0f,-5.0f},{},{1.0f,0.0f} },	//19
		//上
		{{-5.0f, 5.0f,-5.0f},{},{0.0f,1.0f} },	//20
		{{-5.0f, 5.0f, 5.0f},{},{0.0f,0.0f} },	//21
		{{ 5.0f, 5.0f,-5.0f},{},{1.0f,1.0f} },	//22
		{{ 5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },	//23
	};
	//インデックスデータ
	unsigned short indices[] =
	{

		//前
		0,1,2,	//三角形1つ目
		2,1,3,	//三角形2つ目
		//後
		4,5,6,
		6,5,7,
		//左
		8,9,10,
		10,9,11,
		//右
		12,14,13,
		13,14,15,
		//下
		16,17,18,
		18,17,19,
		//上
		20,21,22,
		22,21,23,

		/*4,5,2,
		5,2,3,*/
	};

	//法線の計算
	for (int i = 0; i < _countof(indices) / 3; i++)
	{//三角形1つごとに計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short indices0 = indices[i * 3 + 0];
		unsigned short indices1 = indices[i * 3 + 1];
		unsigned short indices2 = indices[i * 3 + 2];
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//p0→p1ベクトル、p0→p2ベクトルを計算　(ベクトルの減算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化
		normal = XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[indices0].normalize, normal);
		XMStoreFloat3(&vertices[indices1].normalize, normal);
		XMStoreFloat3(&vertices[indices2].normalize, normal);
	}

	//頂点データ全体のサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};	//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//CPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;	//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの生成
	ComPtr<ID3D12Resource> vertBuff;
	result = device->CreateCommittedResource(
		&heapProp,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,	//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));


	//インデックスバッファ
	//インデックスバッファ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	//リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;	//インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	ComPtr<ID3D12Resource> indexBuff;
	result = device->CreateCommittedResource(
		&heapProp,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,	//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	//頂点バッファへのデータ転送
	//GPU上のバッファに対応した仮想メモリ（メインメモリ上）を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];	//座標をコピー
	}
	//つながりを解除
	vertBuff->Unmap(0, nullptr);

	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];	//インデックスをコピー
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);


	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	//頂点１つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);

	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;


	ComPtr<ID3DBlob> vsBlob;	//頂点シェーダーオブジェクト
	ComPtr<ID3DBlob> errorBlob;	//エラーオブジェクト

	//頂点シェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",	//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//インクルード可能にする
		"main",	//エントリー名
		"vs_5_0",	//シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&vsBlob,
		&errorBlob
	);

	//エラーなら
	if (FAILED(result))
	{
		//errorBlobからエラーの内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin()
		);
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	ComPtr<ID3DBlob> psBlob;	//ピクセルシェーダーオブジェクト

	//ピクセルシェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPS.hlsl",	//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//インクルード可能にする
		"main",		//エントリーポイント名
		"ps_5_0",	//シェーダモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&psBlob,
		&errorBlob
	);

	//エラーなら
	if (FAILED(result))
	{
		//errorBlobからエラーの内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin()
		);
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{	//xyz座標
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		//normalize
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		//uv
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	//グラフィックスパイプライン
	//グラフィックスパイプラインの各ステージの設定をする構造体
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//シェーダをパイプラインに設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//標準設定

	//ラスタライザの設定
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//カリングしない
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;	//背面をカリング
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//ポリゴン塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;	//深度クリッピングを有効に

	//ブレンドステートを有効に
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask =
	//	D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定　ブレンドを細かく設定できるようにする
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
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
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	pipelineDesc.NumRenderTargets = 1;	//描画対象の数
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;	//1ピクセルにつき1回サンプリング

	//リソース設定
	D3D12_RESOURCE_DESC depthResorceDesc{};
	depthResorceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResorceDesc.Width = window_width;	//レンダーターゲットに合わせる
	depthResorceDesc.Height = window_height;	//レンダーターゲットに合わせる
	depthResorceDesc.DepthOrArraySize = 1;
	depthResorceDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	depthResorceDesc.SampleDesc.Count = 1;
	depthResorceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//デプスステンシル
	//震度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	//深度値1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット

	//リソース生成
	ComPtr<ID3D12Resource> depthBuff;
	result = device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResorceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff)
	);

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;	//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//デプスステンシルビュー
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//深度ステンシルビューの生成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//デプスステンシルステートの設定
	pipelineDesc.DepthStencilState.DepthEnable = true;	//深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;	//一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[0].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
	//テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//全てのシェーダから見える
	//定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[2].Descriptor.ShaderRegister = 1;					//定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダーから見える


	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;		//横繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;		//横繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;		//奥行繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;						//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;									//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//ピクセルシェーダからのみ使用可能

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;	//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);		//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	assert(SUCCEEDED(result));
	result = device->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);
	assert(SUCCEEDED(result));
	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature.Get();

	//パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelineState;
	result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
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

	ComPtr<ID3D12Resource> constBuffMaterial;
	//定数バッファの生成
	result = device->CreateCommittedResource(
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

	//3Dオブジェクトの数
	const size_t kMetaCount = 3;

	//画像
	TexMetadata metadata[kMetaCount];
	ScratchImage scratchImg[kMetaCount];
	ComPtr<ID3D12DescriptorHeap> srvHeap[kMetaCount];

	//WICテクスチャのロード
	result = LoadFromWICFile(L"Resources/texture.jpg", WIC_FLAGS_NONE, &metadata[0], scratchImg[0]);
	result = LoadFromWICFile(L"Resources/texture2.jpg", WIC_FLAGS_NONE, &metadata[1], scratchImg[1]);
	result = LoadFromWICFile(L"Resources/texture3.jpg", WIC_FLAGS_NONE, &metadata[2], scratchImg[2]);

	//初期化
	for (int i = 0; i < _countof(metadata); i++)
	{
		InitializeMetadata(metadata[i], &scratchImg[i], device.Get(), srvHeap[i].Get(), i + 1);
	}

	////画像
	//TexMetadata metadata{}, metadata2{}, metadata3{};
	//ScratchImage scratchImg{}, scratchImg2{}, scratchImg3{};
	/*ScratchImage mipChain{}, mipChain2{}, mipChain3{};*/
	////ミップマップ生成
	//result = GenerateMipMaps(
	//	scratchImg.GetImages(),
	//	scratchImg.GetImageCount(),
	//	scratchImg.GetMetadata(),
	//	TEX_FILTER_DEFAULT,
	//	0,
	//	mipChain
	//);
	//if (SUCCEEDED(result))
	//{
	//	scratchImg = std::move(mipChain);
	//	metadata = scratchImg.GetMetadata();
	//}
	////読み込んだディフューズテクスチャをSRGBとして扱う
	//metadata.format = MakeSRGB(metadata.format);
	////2
	//result = GenerateMipMaps(
	//	scratchImg2.GetImages(),
	//	scratchImg2.GetImageCount(),
	//	scratchImg2.GetMetadata(),
	//	TEX_FILTER_DEFAULT,
	//	0,
	//	mipChain2
	//);
	//if (SUCCEEDED(result))
	//{
	//	scratchImg2 = std::move(mipChain2);
	//	metadata2 = scratchImg2.GetMetadata();
	//}
	////読み込んだディフューズテクスチャをSRGBとして扱う
	//metadata2.format = MakeSRGB(metadata2.format);
	////3
	//result = GenerateMipMaps(
	//	scratchImg3.GetImages(),
	//	scratchImg3.GetImageCount(),
	//	scratchImg3.GetMetadata(),
	//	TEX_FILTER_DEFAULT,
	//	0,
	//	mipChain3
	//);
	//if (SUCCEEDED(result))
	//{
	//	scratchImg3 = std::move(mipChain3);
	//	metadata3 = scratchImg3.GetMetadata();
	//}
	////読み込んだディフューズテクスチャをSRGBとして扱う
	//metadata3.format = MakeSRGB(metadata3.format);


	////テクスチャバッファ設定
	////ヒープ設定
	//D3D12_HEAP_PROPERTIES textureHeapProp{};
	//textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	//textureHeapProp.CPUPageProperty =
	//	D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	////リソース設定
	//D3D12_RESOURCE_DESC textureResourceDesc{},textureResourceDesc2{},textureResourceDesc3{};
	//textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//textureResourceDesc.Format = metadata.format;
	//textureResourceDesc.Width = metadata.width;	//幅
	//textureResourceDesc.Height = metadata.height;	//高さ
	//textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	//textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	//textureResourceDesc.SampleDesc.Count = 1;

	//textureResourceDesc2.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//textureResourceDesc2.Format = metadata2.format;
	//textureResourceDesc2.Width = metadata2.width;	//幅
	//textureResourceDesc2.Height = metadata2.height;	//高さ
	//textureResourceDesc2.DepthOrArraySize = (UINT16)metadata2.arraySize;
	//textureResourceDesc2.MipLevels = (UINT16)metadata2.mipLevels;
	//textureResourceDesc2.SampleDesc.Count = 1;

	//textureResourceDesc3.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//textureResourceDesc3.Format = metadata3.format;
	//textureResourceDesc3.Width = metadata3.width;	//幅
	//textureResourceDesc3.Height = metadata3.height;	//高さ
	//textureResourceDesc3.DepthOrArraySize = (UINT16)metadata3.arraySize;
	//textureResourceDesc3.MipLevels = (UINT16)metadata3.mipLevels;
	//textureResourceDesc3.SampleDesc.Count = 1;

	////テクスチャバッファの生成
	//ComPtr<ID3D12Resource> texBuff, texBuff2, texBuff3;
	//result = device->CreateCommittedResource(
	//	&textureHeapProp,
	//	D3D12_HEAP_FLAG_NONE,
	//	&textureResourceDesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&texBuff)
	//);
	//result = device->CreateCommittedResource(
	//	&textureHeapProp,
	//	D3D12_HEAP_FLAG_NONE,
	//	&textureResourceDesc2,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&texBuff2)
	//);
	//result = device->CreateCommittedResource(
	//	&textureHeapProp,
	//	D3D12_HEAP_FLAG_NONE,
	//	&textureResourceDesc3,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&texBuff3)
	//);

	////全ミップマップについて
	//for (size_t i = 0; i < metadata2.mipLevels; i++)
	//{
	//	//ミップマップレベルを指定してイメージを取得
	//	const Image* img = scratchImg.GetImage(i, 0, 0);
	//	//テクスチャバッファにデータを転送
	//	result = texBuff->WriteToSubresource(
	//		(UINT)i,
	//		nullptr,				//全領域へコピー
	//		img->pixels,			//元データアドレス
	//		(UINT)img->rowPitch,	//1ラインサイズ
	//		(UINT)img->slicePitch	//1枚サイズ
	//	);
	//	assert(SUCCEEDED(result));
	//}
	////2
	//for (size_t i = 0; i < metadata2.mipLevels; i++)
	//{
	//	//ミップマップレベルを指定してイメージを取得
	//	const Image* img = scratchImg2.GetImage(i, 0, 0);
	//	//テクスチャバッファにデータを転送
	//	result = texBuff2->WriteToSubresource(
	//		(UINT)i,
	//		nullptr,				//全領域へコピー
	//		img->pixels,			//元データアドレス
	//		(UINT)img->rowPitch,	//1ラインサイズ
	//		(UINT)img->slicePitch	//1枚サイズ
	//	);
	//	assert(SUCCEEDED(result));
	//}
	////3
	//for (size_t i = 0; i < metadata3.mipLevels; i++)
	//{
	//	//ミップマップレベルを指定してイメージを取得
	//	const Image* img = scratchImg3.GetImage(i, 0, 0);
	//	//テクスチャバッファにデータを転送
	//	result = texBuff3->WriteToSubresource(
	//		(UINT)i,
	//		nullptr,				//全領域へコピー
	//		img->pixels,			//元データアドレス
	//		(UINT)img->rowPitch,	//1ラインサイズ
	//		(UINT)img->slicePitch	//1枚サイズ
	//	);
	//	assert(SUCCEEDED(result));
	//}

	////元データ解放
	//delete[] imageData;


	////デスクリプタヒープ生成
	////SRVの最大個数
	//const size_t kMaxSRVCount = 2056;

	////デスクリプタヒープの設定
	//D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	//srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	//srvHeapDesc.NumDescriptors = kMaxSRVCount;

	////設定をもとにSRV用デスクリプタヒープを生成
	//ID3D12DescriptorHeap* srvHeap = nullptr;
	//result = device->CreateDescriptorHeap(
	//	&srvHeapDesc,
	//	IID_PPV_ARGS(&srvHeap)
	//);
	//assert(SUCCEEDED(result));

	////SRVヒープの先頭ハンドルを取得
	//D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	////シェーダリソースビュー
	////シェーダリソースビュー設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体
	//srvDesc.Format = textureResourceDesc.Format;
	//srvDesc.Shader4ComponentMapping =
	//	D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	//srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;
	////ハンドルの指す位置にシェーダリソースビュー作成
	//device->CreateShaderResourceView(texBuff.Get(), &srvDesc, srvHandle);

	////2
	//UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//srvHandle.ptr += incrementSize;
	////シェーダリソースビュー設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};	//設定構造体
	//srvDesc2.Format = textureResourceDesc2.Format;
	//srvDesc2.Shader4ComponentMapping =
	//	D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	//srvDesc2.Texture2D.MipLevels = textureResourceDesc2.MipLevels;
	////ハンドルの指す位置にシェーダリソースビュー作成
	//device->CreateShaderResourceView(texBuff2.Get(), &srvDesc2, srvHandle);

	////3
	//srvHandle.ptr += incrementSize;
	////シェーダリソースビュー設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc3{};	//設定構造体
	//srvDesc3.Format = textureResourceDesc3.Format;
	//srvDesc3.Shader4ComponentMapping =
	//	D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc3.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	//srvDesc3.Texture2D.MipLevels = textureResourceDesc3.MipLevels;
	////ハンドルの指す位置にシェーダリソースビュー作成
	//device->CreateShaderResourceView(texBuff3.Get(), &srvDesc3, srvHandle);

	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap[0]->GetGPUDescriptorHandleForHeapStart();
	//保存用変数
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle2 = srvHeap[0]->GetGPUDescriptorHandleForHeapStart();

	//3Dオブジェクトの数
	const size_t kObjectCount = 2;
	//3Dオブジェクトの配列
	Object3d object3ds[kObjectCount];

	//配列内すべてのオブジェクトに対して
	for (int i = 0; i < _countof(object3ds); i++)
	{
		//初期化
		InitializeObject3d(&object3ds[i], device.Get());

		//ここから↓は親子構造のサンプル
		if (i > 0)
		{
			//ひとつ前のオブジェクトを親オブジェクトとする
			object3ds[i].parent = &object3ds[i - 1];
			//親オブジェクトを元にアフィン変換情報を生成
			object3ds[i].scale = { 1.0f,1.0f,1.0f };
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			object3ds[i].position = { -20.0f,0.0f,-8.0f};
		}
	}

	//射影変換
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),			//上下画角45度
		(float)window_width / window_height,//アスペクト比(画面横幅/画面立幅)
		0.1f, 1000.0f						//前端、奥端
	);

	//ビュー変換行列
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);
	XMFLOAT3 target(0, 0, 0);
	XMFLOAT3 up(0, 1, 0);
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

#pragma endregion

	//ゲームループ
	while (true)
	{
#pragma region メッセージ処理

		//メッセージがある？
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	//キー入力メッセージの処理
			DispatchMessage(&msg);	//プロシージャにメッセージを送る
		}

		//Xボタンで終了メッセ時が来たらゲームループを抜ける
		if (msg.message == WM_QUIT)
		{
			break;
		}
#pragma endregion

#pragma region DirectX毎フレーム処理

		//キーボード情報の取得開始
		keyboard->Acquire();
		//全キーの入力状態を取得する
		BYTE key[256] = {};
		keyboard->GetDeviceState(sizeof(key), key);

		for (int i = 0; i < _countof(object3ds); i++)
		{
			UpdateObject3d(&object3ds[i], matView, matProjection);
		}

		if (key[DIK_D] || key[DIK_A] || key[DIK_W] || key[DIK_S])
		{
			if (key[DIK_D]) {
				object3ds[0].rotation.y += XMConvertToRadians(1.0f);
			}
			else if (key[DIK_A]) {
				object3ds[0].rotation.y -= XMConvertToRadians(1.0f);
			}
			if (key[DIK_S]) {
				object3ds[0].rotation.x += XMConvertToRadians(1.0f);
			}
			else if (key[DIK_W]) {
				object3ds[0].rotation.x -= XMConvertToRadians(1.0f);
			}
		}

		//座標を移動する処理
		if (key[DIK_UP] || key[DIK_DOWN] || key[DIK_RIGHT] || key[DIK_LEFT])
		{
			if (key[DIK_UP]) { object3ds[0].position.z += 1.0f; }
			else if (key[DIK_DOWN]) { object3ds[0].position.z -= 1.0f; }
			if (key[DIK_RIGHT]) { object3ds[0].position.x += 1.0f; }
			else if (key[DIK_LEFT]) { object3ds[0].position.x -= 1.0f; }

		}

		//バックバッファの番号を取得(2つなので0番か1番)
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

		// 1. リソースバリアに書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbIndex].Get();	//バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;	//表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	//描画状態へ
		commandList->ResourceBarrier(1, &barrierDesc);

		// 2. 描画先の変更
		// レンダーターゲットビューのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		// 3. 画面クリアコマンド   R     G    B    A
		FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f };
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// 4. 描画コマンド

		//ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};
		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		//ビューポート設定コマンドをコマンドリストに積む
		commandList->RSSetViewports(1, &viewport);

		//シザー矩形
		D3D12_RECT scissorRect{};
		//切り抜き座標
		scissorRect.left = 0;
		scissorRect.right = scissorRect.left + window_width;
		scissorRect.top = 0;
		scissorRect.bottom = scissorRect.top + window_height;
		//シザー矩形設定コマンドをコマンドリストに積む
		commandList->RSSetScissorRects(1, &scissorRect);

		//パイプラインステートをセット
		commandList->SetPipelineState(pipelineState.Get());
		//ルートシグネチャをセット
		commandList->SetGraphicsRootSignature(rootSignature.Get());

		//プリミティブ形状の設定コマンド
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//三角形リスト
		//定数バッファビュー(CBV)の設定コマンド
		commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
		//SRVヒープの設定コマンド
		for (int i = 0; i < _countof(metadata); i++)
		{
			commandList->SetDescriptorHeaps(1, &srvHeap[i]);
		}

		////押したキーによって画像を変更
		//if (key[DIK_1] || key[DIK_2] || key[DIK_3])
		//{
		//	if (key[DIK_1])
		//	{
		//		srvGpuHandle.ptr = srvGpuHandle2.ptr;
		//	}
		//	if (key[DIK_2])
		//	{
		//		srvGpuHandle.ptr = srvGpuHandle2.ptr + incrementSize;
		//	}
		//	if (key[DIK_3])
		//	{
		//		srvGpuHandle.ptr = srvGpuHandle2.ptr + incrementSize * 2;
		//	}
		//}
		//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
		commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		//全オブジェクトを描画
		for (int i = 0; i < _countof(object3ds); i++)
		{
			DrawObject3d(&object3ds[i], commandList.Get(), vbView, ibView, _countof(indices));
		}

		// 5. リソースバリアを書き込み禁止に
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			//表示状態へ
		commandList->ResourceBarrier(1, &barrierDesc);

		//命令のクローズ
		result = commandList->Close();
		assert(SUCCEEDED(result));
		//コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList.Get()};
		commandQueue->ExecuteCommandLists(1, commandLists);

		//画面に表示するバッファをクリップ
		result = swapChain->Present(1, 0);
		assert(SUCCEEDED(result));


		//コマンドの実行完了を待つ
		commandQueue->Signal(fence.Get(), ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			if (event != NULL) {
				WaitForSingleObject(event, INFINITE);
			}
			if (event != NULL) {
				CloseHandle(event);
			}
		}

		//キューをクリア
		result = commandAllocator->Reset();
		assert(SUCCEEDED(result));
		//再びコマンドリストを貯める準備
		result = commandList->Reset(commandAllocator.Get(), nullptr);
		assert(SUCCEEDED(result));

#pragma endregion

		//ウィンドウクラスを登録解除
		UnregisterClass(w.lpszClassName, w.hInstance);
	}

	return 0;
}
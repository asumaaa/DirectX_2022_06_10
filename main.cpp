#include "main.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ウィンドウ生成
	WinApp* win = nullptr;
	win = WinApp::GetInstance();
	win->CreateWindow_(L"DirectX");

	Masage* masage;	//メッセージ
	masage = Masage::GetInstance();

	//DirectX初期化処理
	DirectXIni* dx = nullptr;
	dx = DirectXIni::GetInstance();
	dx->Initialize(win);

	//キーボード
	Input* input = nullptr;
	input = Input::GetInstance();
	input->Initialize(win);

#pragma region 描画処理初期化
	Square2* square = nullptr;
	square = Square2::GetInstance();
	square->Initialize(XMFLOAT3(10.0f, 10.0f, 10.0f),dx);

	Triangle* triangle = nullptr;
	triangle = Triangle::GetInstance();
	triangle->Initialize(XMFLOAT3(10.0f,10.0f,10.0f),dx);

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
	result = dx->GetDevice()->CreateCommittedResource(
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
	result = dx->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//深度ステンシルビューの生成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dx->GetDevice()->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//読み込む画像の数
	const size_t metadataCount = 3;
	//画像
	Texture texture[metadataCount];
	//初期化
	texture[0].Initialize(L"Resources/texture.jpg", dx, 0);
	texture[1].Initialize(L"Resources/texture2.jpg", dx, 1);
	texture[2].Initialize(L"Resources/texture3.jpg", dx, 2);


	//3Dオブジェクトの数
	const size_t kObjectCount = 2;
	//3Dオブジェクトの配列
	Object3d object3ds[kObjectCount];

	//配列内すべてのオブジェクトに対して
	for (int i = 0; i < _countof(object3ds); i++)
	{
		//初期化
		InitializeObject3d(&object3ds[i], dx->GetDevice());

		//ここから↓は親子構造のサンプル
		if (i > 0)
		{
			//ひとつ前のオブジェクトを親オブジェクトとする
			object3ds[i].parent = &object3ds[i - 1];
			//親オブジェクトを元にアフィン変換情報を生成
			object3ds[i].scale = { 1.0f,1.0f,1.0f };
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			object3ds[i].position = { -20.0f,0.0f,-8.0f };
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
		//メッセージがある？
		masage->Update();

#pragma region DirectX毎フレーム処理

		//キーボード更新
		input->Update();

		for (int i = 0; i < _countof(object3ds); i++)
		{
			UpdateObject3d(&object3ds[i], matView, matProjection);
		}

		if (input->key[DIK_D] || input->key[DIK_A] || input->key[DIK_W] || input->key[DIK_S])
		{
			if (input->key[DIK_D]) {
				object3ds[0].rotation.y += XMConvertToRadians(1.0f);
			}
			else if (input->key[DIK_A]) {
				object3ds[0].rotation.y -= XMConvertToRadians(1.0f);
			}
			if (input->key[DIK_S]) {
				object3ds[0].rotation.x += XMConvertToRadians(1.0f);
			}
			else if (input->key[DIK_W]) {
				object3ds[0].rotation.x -= XMConvertToRadians(1.0f);
			}
		}

		//座標を移動する処理
		if (input->key[DIK_UP] || input->key[DIK_DOWN] || input->key[DIK_RIGHT] ||input->key[DIK_LEFT])
		{
			if (input->key[DIK_UP]) { object3ds[0].position.z += 1.0f; }
			else if (input->key[DIK_DOWN]) { object3ds[0].position.z -= 1.0f; }
			if (input->key[DIK_RIGHT]) { object3ds[0].position.x += 1.0f; }
			else if (input->key[DIK_LEFT]) { object3ds[0].position.x -= 1.0f; }

		}

		//バックバッファの番号を取得(2つなので0番か1番)
		UINT bbIndex = dx->GetSwapChain()->GetCurrentBackBufferIndex();

		// 1. リソースバリアに書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = dx->backBuffers[bbIndex].Get();	//バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;	//表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	//描画状態へ
		dx->GetCommandList()->ResourceBarrier(1, &barrierDesc);

		// 2. 描画先の変更
		// レンダーターゲットビューのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = dx->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * dx->GetDevice()->GetDescriptorHandleIncrementSize(dx->rtvHeapDesc.Type);
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		dx->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		// 3. 画面クリアコマンド   R     G    B    A
		FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f };
		dx->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		dx->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// 4. 描画コマンド
		square->Update();
		triangle->Update();

		//seikinを描画
		texture[2].Draw();
		DrawObject3d(&object3ds[0], dx->GetCommandList(), square->vertBuff.vbView, square->indexBuff.ibView, _countof(square->vertex->indices));

		//hikakinを描画
		texture[1].Draw();
		DrawObject3d(&object3ds[1], dx->GetCommandList(), triangle->vertBuff.vbView, triangle->indexBuff.ibView, _countof(triangle->vertex->indices));

		// 5. リソースバリアを書き込み禁止に
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			//表示状態へ
		dx->GetCommandList()->ResourceBarrier(1, &barrierDesc);

		//命令のクローズ
		result = dx->GetCommandList()->Close();
		assert(SUCCEEDED(result));
		//コマンドリストの実行
		ID3D12CommandList* commandLists[] = { dx->GetCommandList() };
		dx->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

		//画面に表示するバッファをクリップ
		result = dx->GetSwapChain()->Present(1, 0);
		assert(SUCCEEDED(result));


		//コマンドの実行完了を待つ
		dx->GetCommandQueue()->Signal(dx->GetFence(), ++dx->fenceVal);
		if (dx->GetFence()->GetCompletedValue() != dx->fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			dx->GetFence()->SetEventOnCompletion(dx->fenceVal, event);
			if (event != NULL) {
				WaitForSingleObject(event, INFINITE);
			}
			if (event != NULL) {
				CloseHandle(event);
			}
		}

		//キューをクリア
		result = dx->GetCommandAllocator()->Reset();
		assert(SUCCEEDED(result));
		//再びコマンドリストを貯める準備
		result = dx->GetCommandList()->Reset(dx->GetCommandAllocator(), nullptr);
		assert(SUCCEEDED(result));

#pragma endregion


		//Xボタンで終了メッセ時が来たらゲームループを抜ける 
		if (masage->ExitGameloop() == 1)
		{
			break;
		}

		//ウィンドウクラスを登録解除
		win->deleteWindow();
	}

	return 0;
}
#include "main.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//�E�B���h�E����
	WinApp* win = nullptr;
	win = WinApp::GetInstance();
	win->CreateWindow_(L"DirectX");

	Masage* masage;	//���b�Z�[�W
	masage = Masage::GetInstance();

	//DirectX����������
	DirectXIni* dx = nullptr;
	dx = DirectXIni::GetInstance();
	dx->Initialize(win);

	//�L�[�{�[�h
	Input* input = nullptr;
	input = Input::GetInstance();
	input->Initialize(win);

#pragma region �`�揈��������
	Square2* square = nullptr;
	square = Square2::GetInstance();
	square->Initialize(XMFLOAT3(10.0f, 10.0f, 10.0f),dx);

	Triangle* triangle = nullptr;
	triangle = Triangle::GetInstance();
	triangle->Initialize(XMFLOAT3(10.0f,10.0f,10.0f),dx);

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC depthResorceDesc{};
	depthResorceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResorceDesc.Width = window_width;	//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResorceDesc.Height = window_height;	//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResorceDesc.DepthOrArraySize = 1;
	depthResorceDesc.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g
	depthResorceDesc.SampleDesc.Count = 1;
	depthResorceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//�f�v�X�X�e���V��

	//�k�x�l�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//�[�x�l�̃N���A�ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	//�[�x�l1.0f(�ő�l)�ŃN���A
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g

	//���\�[�X����
	ComPtr<ID3D12Resource> depthBuff;
	result = dx->GetDevice()->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResorceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//�[�x�l�������݂Ɏg�p
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff)
	);

	//�[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;	//�[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//�f�v�X�X�e���V���r���[
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	result = dx->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//�[�x�X�e���V���r���[�̐���
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dx->GetDevice()->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//�ǂݍ��މ摜�̐�
	const size_t metadataCount = 3;
	//�摜
	Texture texture[metadataCount];
	//������
	texture[0].Initialize(L"Resources/texture.jpg", dx, 0);
	texture[1].Initialize(L"Resources/texture2.jpg", dx, 1);
	texture[2].Initialize(L"Resources/texture3.jpg", dx, 2);


	//3D�I�u�W�F�N�g�̐�
	const size_t kObjectCount = 2;
	//3D�I�u�W�F�N�g�̔z��
	Object3d object3ds[kObjectCount];

	//�z������ׂẴI�u�W�F�N�g�ɑ΂���
	for (int i = 0; i < _countof(object3ds); i++)
	{
		//������
		InitializeObject3d(&object3ds[i], dx->GetDevice());

		//�������火�͐e�q�\���̃T���v��
		if (i > 0)
		{
			//�ЂƂO�̃I�u�W�F�N�g��e�I�u�W�F�N�g�Ƃ���
			object3ds[i].parent = &object3ds[i - 1];
			//�e�I�u�W�F�N�g�����ɃA�t�B���ϊ����𐶐�
			object3ds[i].scale = { 1.0f,1.0f,1.0f };
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			object3ds[i].position = { -20.0f,0.0f,-8.0f };
		}
	}

	//�ˉe�ϊ�
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),			//�㉺��p45�x
		(float)window_width / window_height,//�A�X�y�N�g��(��ʉ���/��ʗ���)
		0.1f, 1000.0f						//�O�[�A���[
	);

	//�r���[�ϊ��s��
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);
	XMFLOAT3 target(0, 0, 0);
	XMFLOAT3 up(0, 1, 0);
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

#pragma endregion

	//�Q�[�����[�v
	while (true)
	{
		//���b�Z�[�W������H
		masage->Update();

#pragma region DirectX���t���[������

		//�L�[�{�[�h�X�V
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

		//���W���ړ����鏈��
		if (input->key[DIK_UP] || input->key[DIK_DOWN] || input->key[DIK_RIGHT] ||input->key[DIK_LEFT])
		{
			if (input->key[DIK_UP]) { object3ds[0].position.z += 1.0f; }
			else if (input->key[DIK_DOWN]) { object3ds[0].position.z -= 1.0f; }
			if (input->key[DIK_RIGHT]) { object3ds[0].position.x += 1.0f; }
			else if (input->key[DIK_LEFT]) { object3ds[0].position.x -= 1.0f; }

		}

		//�o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
		UINT bbIndex = dx->GetSwapChain()->GetCurrentBackBufferIndex();

		// 1. ���\�[�X�o���A�ɏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = dx->backBuffers[bbIndex].Get();	//�o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;	//�\����Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	//�`���Ԃ�
		dx->GetCommandList()->ResourceBarrier(1, &barrierDesc);

		// 2. �`���̕ύX
		// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = dx->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * dx->GetDevice()->GetDescriptorHandleIncrementSize(dx->rtvHeapDesc.Type);
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		dx->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		// 3. ��ʃN���A�R�}���h   R     G    B    A
		FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f };
		dx->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		dx->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// 4. �`��R�}���h
		square->Update();
		triangle->Update();

		//seikin��`��
		texture[2].Draw();
		DrawObject3d(&object3ds[0], dx->GetCommandList(), square->vertBuff.vbView, square->indexBuff.ibView, _countof(square->vertex->indices));

		//hikakin��`��
		texture[1].Draw();
		DrawObject3d(&object3ds[1], dx->GetCommandList(), triangle->vertBuff.vbView, triangle->indexBuff.ibView, _countof(triangle->vertex->indices));

		// 5. ���\�[�X�o���A���������݋֎~��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//�`���Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			//�\����Ԃ�
		dx->GetCommandList()->ResourceBarrier(1, &barrierDesc);

		//���߂̃N���[�Y
		result = dx->GetCommandList()->Close();
		assert(SUCCEEDED(result));
		//�R�}���h���X�g�̎��s
		ID3D12CommandList* commandLists[] = { dx->GetCommandList() };
		dx->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

		//��ʂɕ\������o�b�t�@���N���b�v
		result = dx->GetSwapChain()->Present(1, 0);
		assert(SUCCEEDED(result));


		//�R�}���h�̎��s������҂�
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

		//�L���[���N���A
		result = dx->GetCommandAllocator()->Reset();
		assert(SUCCEEDED(result));
		//�ĂуR�}���h���X�g�𒙂߂鏀��
		result = dx->GetCommandList()->Reset(dx->GetCommandAllocator(), nullptr);
		assert(SUCCEEDED(result));

#pragma endregion


		//X�{�^���ŏI�����b�Z����������Q�[�����[�v�𔲂��� 
		if (masage->ExitGameloop() == 1)
		{
			break;
		}

		//�E�B���h�E�N���X��o�^����
		win->deleteWindow();
	}

	return 0;
}
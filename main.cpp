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

	Ver* vertex = nullptr;
	vertex = Ver::GetInstance();
	vertex->Initialize(XMFLOAT3(5.0, 5.0, 5.0));
	IndexBuff indexBuff;
	indexBuff.GetInstance();
	indexBuff.Initialize(vertex, dx);
	VertBuff vertBuff;
	vertBuff.GetInstance();
	vertBuff.Initialize(vertex, dx);
	Shader shader;
	shader.GetInstance();
	shader.compileVs(L"BasicVS.hlsl");
	shader.compilePs(L"BasicPS.hlsl");

	//�O���t�B�b�N�X�p�C�v���C��
	//�O���t�B�b�N�X�p�C�v���C���̊e�X�e�[�W�̐ݒ������\����
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//�V�F�[�_���p�C�v���C���ɐݒ�
	pipelineDesc.VS.pShaderBytecode = shader.vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = shader.vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = shader.psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = shader.psBlob->GetBufferSize();

	//�T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//�W���ݒ�

	//���X�^���C�U�̐ݒ�
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//�J�����O���Ȃ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;	//�w�ʂ��J�����O
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//�|���S���h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true;	//�[�x�N���b�s���O��L����

	//�u�����h�X�e�[�g��L����
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask =
	//	D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBA�S�Ẵ`�����l����`��

	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�@�u�����h���ׂ����ݒ�ł���悤�ɂ���
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBA�S�Ẵ`�����l����`��

	//�u�����h�ݒ�
	//���ʐݒ�(�A���t�@�l)
	blenddesc.BlendEnable = true;					//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//�f�X�g�̒l��0  %�g��
	////���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;	//���Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;		//�\�[�X�̒l��100%�g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE;	//�f�X�g�̒l��100%�g��
	////���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;	//�f�X�g����\�[�X�����Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;				//�\�[�X�̒l��100%�g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE;				//�f�X�g�̒l��100%�g��
	////�F���]
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//���Z
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	//1.0f-�f�X�g�J���[�̒l
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;				//�g��Ȃ�
	//����������
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = vertex->inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(vertex->inputLayout);

	//�}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1;	//�`��Ώۂ̐�
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1;	//1�s�N�Z���ɂ�1��T���v�����O

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

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	pipelineDesc.DepthStencilState.DepthEnable = true;	//�[�x�e�X�g���s��
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//�������݋���
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;	//��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//�萔�o�b�t�@0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
	rootParams[0].Descriptor.ShaderRegister = 0;					//�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			//�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//�f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//�S�ẴV�F�[�_���猩����
	//�萔�o�b�t�@1��
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
	rootParams[2].Descriptor.ShaderRegister = 1;					//�萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_�[���猩����


	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;

	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;		//���J��Ԃ�
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;		//���J��Ԃ�
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;		//���s�J��Ԃ�
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//�S�ă��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;						//�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;									//�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//�s�N�Z���V�F�[�_����̂ݎg�p�\

	//���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;	//���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);		//���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&shader.errorBlob);
	assert(SUCCEEDED(result));
	result = dx->GetDevice()->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);
	assert(SUCCEEDED(result));
	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature.Get();

	//�p�C�v���C���X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelineState;
	result = dx->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;	//256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ComPtr<ID3D12Resource> constBuffMaterial;
	//�萔�o�b�t�@�̐���
	result = dx->GetDevice()->CreateCommittedResource(
		&cbHeapProp,	//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,	//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial)
	);
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//�}�b�s���O
	assert(SUCCEEDED(result));
	//�l���������ނƎ����I�ɓ]�������
	constMapMaterial->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

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

		//�r���[�|�[�g�ݒ�R�}���h
		D3D12_VIEWPORT viewport{};
		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		//�r���[�|�[�g�ݒ�R�}���h���R�}���h���X�g�ɐς�
		dx->GetCommandList()->RSSetViewports(1, &viewport);

		//�V�U�[��`
		D3D12_RECT scissorRect{};
		//�؂蔲�����W
		scissorRect.left = 0;
		scissorRect.right = scissorRect.left + window_width;
		scissorRect.top = 0;
		scissorRect.bottom = scissorRect.top + window_height;
		//�V�U�[��`�ݒ�R�}���h���R�}���h���X�g�ɐς�
		dx->GetCommandList()->RSSetScissorRects(1, &scissorRect);

		//�p�C�v���C���X�e�[�g���Z�b�g
		dx->GetCommandList()->SetPipelineState(pipelineState.Get());
		//���[�g�V�O�l�`�����Z�b�g
		dx->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

		//�v���~�e�B�u�`��̐ݒ�R�}���h
		dx->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//�O�p�`���X�g
		//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
		dx->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		//seikin��`��
		texture[2].Draw();
		DrawObject3d(&object3ds[0], dx->GetCommandList(), vertBuff.vbView, indexBuff.ibView, _countof(vertex->indices));

		//hikakin��`��
		texture[1].Draw();
		DrawObject3d(&object3ds[1], dx->GetCommandList(), vertBuff.vbView, indexBuff.ibView, _countof(vertex->indices));

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
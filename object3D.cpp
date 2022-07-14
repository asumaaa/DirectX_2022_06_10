#include "object3D.h"

//3D�I�u�W�F�N�g������
void InitializeObject3d(Object3d* object, ComPtr<ID3D12Device> device)
{
	HRESULT result;

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;	//256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffTransform)
	);
	assert(SUCCEEDED(result));
	//�萔�o�b�t�@�̃}�b�s���O
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

	//���[���h�s��̍���
	object->matWorld = XMMatrixIdentity();
	object->matWorld *= matScale;
	object->matWorld *= matRot;
	object->matWorld *= matTrans;

	//�e�I�u�W�F�N�g�������
	if (object->parent != nullptr)
	{
		object->matWorld *= object->parent->matWorld;
	}

	//�萔�o�b�t�@�փf�[�^�]��
	object->constMapTransform->mat = object->matWorld * matView * matProjection;
}

//�I�u�W�F�N�g�`�揈��
void DrawObject3d(Object3d* object, ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices)
{
	//���_�o�b�t�@�̐ݒ�
	commandList->IASetVertexBuffers(0, 1, &vbView);
	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());
	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetIndexBuffer(&ibView);
	//�`��R�}���h
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);	//�S�Ă̒��_���g���ĕ`��
}

void InitializeTexData(texData* tex, const wchar_t* szFile, ComPtr<ID3D12Device> device,  int texNum)
{
	HRESULT result;
	UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);	//�f�X�N���v�^�̃T�C�Y

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
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	tex->metadata.format = MakeSRGB(tex->metadata.format);

	//�e�N�X�`���o�b�t�@�ݒ�
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = tex->metadata.format;
	textureResourceDesc.Width = tex->metadata.width;	//��
	textureResourceDesc.Height = tex->metadata.height;	//����
	textureResourceDesc.DepthOrArraySize = (UINT16)tex->metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)tex->metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//�e�N�X�`���o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&tex->texBuff)
	);

	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < tex->metadata.mipLevels; i++)
	{
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = tex->scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^��]��
		result = tex->texBuff->WriteToSubresource(
			/*(UINT)*/i,
			nullptr,				//�S�̈�փR�s�[
			img->pixels,			//���f�[�^�A�h���X
			/*(UINT)*/img->rowPitch,	//1���C���T�C�Y
			/*(UINT)*/img->slicePitch	//1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	//��
	//�f�X�N���v�^�q�[�v����
	//SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	//�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_�[���猩����悤��
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = device->CreateDescriptorHeap(
		&srvHeapDesc,
		IID_PPV_ARGS(&tex->srvHeap)
	);
	assert(SUCCEEDED(result));

	//SRV�q�[�v�̐擪�n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = tex->srvHeap->GetCPUDescriptorHandleForHeapStart();
	//�����܂�

	//�e�N�X�`���[�̔ԍ���0�ȍ~�̏ꍇ�n���h����i�߂�
	if (texNum > 0)
	{
		srvHandle.ptr += (incrementSize * texNum);
	}
	//�V�F�[�_���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//�ݒ�\����
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;
	//�n���h���̎w���ʒu�ɃV�F�[�_���\�[�X�r���[�쐬
	device->CreateShaderResourceView(tex->texBuff.Get(), &srvDesc, srvHandle);

	//�n���h���̒l��ݒ�
	tex->srvGpuHandle = tex->srvHeap->GetGPUDescriptorHandleForHeapStart();
	//�ۑ��p�ϐ�
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle2 = tex->srvHeap->GetGPUDescriptorHandleForHeapStart();
	//�e�N�X�`���̔ԍ���0�ȊO�̎�srvGpuHandle�̒l��ύX
	if (texNum > 0)
	{
		tex->srvGpuHandle.ptr = srvGpuHandle2.ptr + (incrementSize * texNum);
	}
}

void DrawTex3d(texData* tex, ComPtr<ID3D12GraphicsCommandList> commandList)
{
	ID3D12DescriptorHeap* ppHeaps[] = { tex->srvHeap.Get() };
	commandList->SetDescriptorHeaps(1, ppHeaps);
	//�`��R�}���h
	commandList->SetGraphicsRootDescriptorTable(1, tex->srvGpuHandle);
}
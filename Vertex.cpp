#include "Vertex.h"

Ver* Ver::GetInstance()
{
	static Ver instance;
	return &instance;
}

void Ver::Initialize(XMFLOAT3 size)
{
	//���_�f�[�^
		Vertex v[] = {
			//�O
			{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//0
			{{-size.x / 2, size.y / 2,-size.z / 2},{},{0.0f,0.0f} },	//1 
			{{ size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//2 
			{{ size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,0.0f} },	//3
			//��				 	   
			{{ size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,1.0f} },	//4
			{{ size.x / 2, size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//5
			{{-size.x / 2,-size.y / 2, size.z / 2},{},{1.0f,1.0f} },	//6
			{{-size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//7
			//��				 	    
			{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//8
			{{-size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//9
			{{-size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//10
			{{-size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//11
			//�E				 	    
			{{ size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//12
			{{ size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//13
			{{ size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//14
			{{ size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//15
			//��					  	
			{{-size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,1.0f} },	//16
			{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,0.0f} },	//17
			{{ size.x / 2,-size.y / 2, size.z / 2},{},{1.0f,1.0f} },	//18
			{{ size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,0.0f} },	//19
			//��				 	    
			{{-size.x / 2, size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//20
			{{-size.x / 2, size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//21
			{{ size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//22
			{{ size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//23
		};
		//�C���f�b�N�X�f�[�^
		unsigned short in[] =
		{

			//�O
			0,1,2,	//�O�p�`1��
			2,1,3,	//�O�p�`2��
			//��
			4,5,6,
			6,5,7,
			//��
			8,9,10,
			10,9,11,
			//�E
			12,14,13,
			13,14,15,
			//��
			16,17,18,
			18,17,19,
			//��
			20,21,22,
			22,21,23,
		};

		//���_���W�Auv���W�A�C���f�b�N�X�f�[�^����
		for (int i = 0; i < 24; i++)
		{
			vertices[i] = v[i];
		}

		for (int i = 0; i < 36; i++)
		{
			indices[i] = in[i];
		}

		//�@���̌v�Z
		for (int i = 0; i < _countof(indices) / 3; i++)
		{//�O�p�`1���ƂɌv�Z���Ă���
			//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
			unsigned short indices0 = indices[i * 3 + 0];
			unsigned short indices1 = indices[i * 3 + 1];
			unsigned short indices2 = indices[i * 3 + 2];
			//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
			XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
			XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
			XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
			//p0��p1�x�N�g���Ap0��p2�x�N�g�����v�Z�@(�x�N�g���̌��Z)
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//�O�ς͗������琂���ȃx�N�g��
			XMVECTOR normal = XMVector3Cross(v1, v2);
			//���K��
			normal = XMVector3Normalize(normal);
			//���߂��@���𒸓_�f�[�^�ɑ��
			XMStoreFloat3(&vertices[indices0].normalize, normal);
			XMStoreFloat3(&vertices[indices1].normalize, normal);
			XMStoreFloat3(&vertices[indices2].normalize, normal);
		}

		sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
		sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

		//���_���C�A�E�g
		D3D12_INPUT_ELEMENT_DESC inputLayout_[] =
		{
			{	//xyz���W
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
		for (int i = 0; i < 3; i++)
		{
			inputLayout[i] = inputLayout_[i];
		}
}

Ver2* Ver2::GetInstance()
{
	static Ver2 instance;
	return &instance;
}

void Ver2::Initialize(XMFLOAT3 size)
{
	//���_�f�[�^
	Vertex v[] = {
		//�O
		{{0          , size.y / 2,          0},{},{0.5f,0.0f} },	//0
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//2 
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//1 
		//��
		{{			0, size.y / 2,          0},{},{0.5f,0.0f} },	//3
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//4
		{{          0,-size.y / 2, size.z / 2},{},{1.0f,1.0f} },	//5 
		//�E
		{{0          , size.y / 2,          0},{},{0.5f,0.0f} },	//6
		{{          0,-size.y / 2, size.z / 2},{},{0.0f,1.0f} },	//7 
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//8 
		//��
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.5f,0.0f} },	//9
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//10 
		{{			0,-size.y / 2, size.z / 2},{},{1.0f,1.0f} },	//11 

		{{0          , size.y / 2,          0},{},{0.5f,0.0f} },	//0
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//2 
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//1 {{0          , size.y / 2,          0},{},{0.5f,0.0f} },	//0
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//2 
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//1 {{0          , size.y / 2,          0},{},{0.5f,0.0f} },	//0
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//2 
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//1 {{0          , size.y / 2,          0},{},{0.5f,0.0f} },	//0
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//2 
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//1 
	};
	//�C���f�b�N�X�f�[�^
	unsigned short in[] =
	{
		//�O
		0,1,2,	//�O�p�`1��
		3,4,5,	//�O�p�`2��
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23,
	};

	//���_���W�Auv���W�A�C���f�b�N�X�f�[�^����
	for (int i = 0; i < 12; i++)
	{
		vertices[i] = v[i];
	}

	for (int i = 0; i < 12; i++)
	{
		indices[i] = in[i];
	}

	//�@���̌v�Z
	for (int i = 0; i < 12 / 3; i++)
	{//�O�p�`1���ƂɌv�Z���Ă���
		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		unsigned short indices0 = indices[i * 3 + 0];
		unsigned short indices1 = indices[i * 3 + 1];
		unsigned short indices2 = indices[i * 3 + 2];
		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//p0��p1�x�N�g���Ap0��p2�x�N�g�����v�Z�@(�x�N�g���̌��Z)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//�O�ς͗������琂���ȃx�N�g��
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//���K��
		normal = XMVector3Normalize(normal);
		//���߂��@���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices[indices0].normalize, normal);
		XMStoreFloat3(&vertices[indices1].normalize, normal);
		XMStoreFloat3(&vertices[indices2].normalize, normal);
	}

	sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
	sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout_[] =
	{
		{	//xyz���W
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
	for (int i = 0; i < 3; i++)
	{
		inputLayout[i] = inputLayout_[i];
	}
}
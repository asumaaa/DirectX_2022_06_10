#pragma once
#include "Windows.h"
#include "d3d12.h"
#include "dxgi1_6.h"
#include "cassert"
#include "vector"
#include "string"
#include "DirectXMath.h"
#include "assert.h"
#include "DirectXTex.h"
#include "wrl.h"

using namespace DirectX;
using namespace Microsoft::WRL;

//���_�f�[�^�\����
struct Vertex
{
	XMFLOAT3 pos;	//���W
	XMFLOAT3 normalize;	//�@���x�N�g��
	XMFLOAT2 uv;	//uv���W
};

struct Indices
{
	int num;
};

class Ver
{
public:
	static Ver* GetInstance();
	void Initialize(XMFLOAT3 size);
public:
	Vertex vertices[24];
	unsigned short indices[36];
	UINT sizeVB;
	UINT sizeIB;
	D3D12_INPUT_ELEMENT_DESC inputLayout[3];//���_���C�A�E�g	xyz���W�A�@���x�N�g���Auv���W�̏���
};

class Ver2
{
public:
	static Ver2* GetInstance();
	void Initialize(XMFLOAT3 size);
public:
	Vertex vertices[12];
	unsigned short indices[12];
	UINT sizeVB;
	UINT sizeIB;
	D3D12_INPUT_ELEMENT_DESC inputLayout[3];//���_���C�A�E�g	xyz���W�A�@���x�N�g���Auv���W�̏���
};
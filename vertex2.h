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

class vertex2
{
public:
	static vertex2* GetInstance();
	void Initialize(XMFLOAT3 size);
public:
	Vertex vertices[24];
	unsigned short indices[24];
	UINT sizeVB;
	UINT sizeIB;

};


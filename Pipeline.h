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

#include "VertBuff.h"
#include "IndexBuff.h"
#include "Shader.h"
#include "Vertex.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Pipeline
{
public:
	void Initialize(Shader shader,Ver* vertex);
public:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc{};//�O���t�B�b�N�X�p�C�v���C���̊e�X�e�[�W�̐ݒ������\����
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc;//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
};


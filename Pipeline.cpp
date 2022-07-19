#include "Pipeline.h"

void Pipeline::Initialize(Shader shader,Ver *vertex)
{

	//�V�F�[�_���p�C�v���C���ɐݒ�
	Desc.VS.pShaderBytecode = shader.vsBlob->GetBufferPointer();
	Desc.VS.BytecodeLength = shader.vsBlob->GetBufferSize();
	Desc.PS.pShaderBytecode = shader.psBlob->GetBufferPointer();
	Desc.PS.BytecodeLength = shader.psBlob->GetBufferSize();

	//�T���v���}�X�N�̐ݒ�
	Desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//�W���ݒ�

	//���X�^���C�U�̐ݒ�
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//�J�����O���Ȃ�
	Desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;	//�w�ʂ��J�����O
	Desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//�|���S���h��Ԃ�
	Desc.RasterizerState.DepthClipEnable = true;	//�[�x�N���b�s���O��L����

	//�u�����h�X�e�[�g��L����
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask =
	//	D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBA�S�Ẵ`�����l����`��

	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�@�u�����h���ׂ����ݒ�ł���悤�ɂ���
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = Desc.BlendState.RenderTarget[0];
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
	Desc.InputLayout.pInputElementDescs = vertex->inputLayout;
	Desc.InputLayout.NumElements = _countof(vertex->inputLayout);

	//�}�`�̌`��ݒ�
	Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//���̑��̐ݒ�
	Desc.NumRenderTargets = 1;	//�`��Ώۂ̐�
	Desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0~255�w���RGBA
	Desc.SampleDesc.Count = 1;	//1�s�N�Z���ɂ�1��T���v�����O
}

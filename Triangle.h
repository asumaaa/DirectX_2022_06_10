#include "Windows.h"
#include "d3d12.h"
#include "dxgi1_6.h"
#include "cassert"
#include "vector"
#include "string"
#include "DirectXMath.h"
#include "d3dcompiler.h"
#include "assert.h"
#include "Vector.h"

using namespace DirectX;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

class Triangle
{
public:
	XMFLOAT3 vertices[3];
private:
	HRESULT result;
	ID3D12Device* device = nullptr;
public:
	//�R�}���h���X�g
	ID3D12GraphicsCommandList* commandList = nullptr;
	//���_�o�b�t�@
	D3D12_HEAP_PROPERTIES heapProp{};
	ID3D12Resource* vertBuff = nullptr;
	XMFLOAT3* vertMap = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�V�F�[�_�t�@�C���I�u�W�F�N�g
	ID3DBlob* vsBlob = nullptr;	//���_�V�F�[�_�[�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr;	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr;	//�G���[�I�u�W�F�N�g
	//�O���t�B�b�N�X�p�C�v���C��
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	//���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	ID3DBlob* rootSigBlob = nullptr;
	//�p�C�v���C���X�e�[�g
	ID3D12PipelineState* pipelineState = nullptr;
public:
	void Initialize(XMFLOAT3 vertices_[3], ID3D12Device* device_);
	//�r���[�|�[�g�̍��W���w��(����x,����y,����,����)
	void Draw(Vector4 viewports, ID3D12GraphicsCommandList* commandList_);
};

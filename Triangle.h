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
	//コマンドリスト
	ID3D12GraphicsCommandList* commandList = nullptr;
	//頂点バッファ
	D3D12_HEAP_PROPERTIES heapProp{};
	ID3D12Resource* vertBuff = nullptr;
	XMFLOAT3* vertMap = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//シェーダファイルオブジェクト
	ID3DBlob* vsBlob = nullptr;	//頂点シェーダーオブジェクト
	ID3DBlob* psBlob = nullptr;	//ピクセルシェーダーオブジェクト
	ID3DBlob* errorBlob = nullptr;	//エラーオブジェクト
	//グラフィックスパイプライン
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	//ルートシグネチャ
	ID3D12RootSignature* rootSignature;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	ID3DBlob* rootSigBlob = nullptr;
	//パイプラインステート
	ID3D12PipelineState* pipelineState = nullptr;
public:
	void Initialize(XMFLOAT3 vertices_[3], ID3D12Device* device_);
	//ビューポートの座標を指定(左上x,左上y,横幅,立幅)
	void Draw(Vector4 viewports, ID3D12GraphicsCommandList* commandList_);
};

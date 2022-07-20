#include "Shader.h"

Shader* Shader::GetInstance()
{
	static Shader instance;
	return &instance;
}

void Shader::compileVs(const wchar_t* file)
{
	ID3DBlob* vsBlob_ = nullptr;	//頂点シェーダーオブジェクト
	ID3DBlob* errorBlob_ = nullptr;	//エラーオブジェクト

	HRESULT result;

	//頂点シェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		file,	//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//インクルード可能にする
		"main",	//エントリー名
		"vs_5_0",	//シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&vsBlob_,
		&errorBlob_
	);

	//エラーなら
	if (FAILED(result))
	{
		//errorBlobからエラーの内容をstring型にコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());

		std::copy_n(
			(char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin()
		);
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	vsBlob = vsBlob_;
	errorBlob = errorBlob_;
}

void Shader::compilePs(const wchar_t* file)
{
	ID3DBlob* psBlob_ = nullptr;	//頂点シェーダーオブジェクト
	ID3DBlob* errorBlob_ = nullptr;	//エラーオブジェクト

	HRESULT result;

	//ピクセルシェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		file,	//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//インクルード可能にする
		"main",		//エントリーポイント名
		"ps_5_0",	//シェーダモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&psBlob_,
		&errorBlob_
	);

	//エラーなら
	if (FAILED(result))
	{
		//errorBlobからエラーの内容をstring型にコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());

		std::copy_n(
			(char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin()
		);
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	psBlob = psBlob_;
	errorBlob = errorBlob_;

}

#pragma once
#include "Windows.h"
#include "d3d12.h"
#include "dxgi1_6.h"
#include "cassert"
#include "vector"
#include "string"
#include "DirectXMath.h"
#include "d3dcompiler.h"
#include "dinput.h"
#include "assert.h"
#include "DirectXTex.h"

#define DIRECTINPUT_VERSION	0x0800	//DirectInputtのバージョン指定

using namespace DirectX;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//構造体
struct Vector3
{
	float x;
	float y;
	float z;
};

class Matrix4 {
public:
	// 行x列
	float m[4][4];

	Matrix4();
	// 成分を指定しての生成
	Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);
	//単位行列で初期化
	void Initialize();

	//ワールド座標系
	//引数でスケールを計算
	void scale(float x, float y, float z);
	//引数で回転を計算
	void rotation(float x,float y,float z);
	//引数で平行移動を計算
	void move(float x, float y, float z);

	//射影変換 画角 アスペクト比 前端 奥端
	void Perspective(float angle,float aspect,float znear,float zfar);

	//ビュー変換
	void View(Vector3 eye, Vector3 target, Vector3 up);

	//Matrix4同士の掛け算
	void matrixUpdate(Matrix4 m2);
};

//vector3と行列の掛け算
void worldMatrixScale(Vector3* vector_, Matrix4* Matrix_);
void worldMatrixMove(Vector3* vector_, Matrix4* Matrix_);
void worldMatrixRoll(Vector3* vector_, Matrix4* Matrix_);
//すべての掛け算をまとめた関数
void vecWorldTransform(Vector3* vector_, Matrix4* Matrix_);

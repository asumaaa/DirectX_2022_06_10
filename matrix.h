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

#define DIRECTINPUT_VERSION	0x0800	//DirectInputt�̃o�[�W�����w��

using namespace DirectX;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//�\����
struct Vector3
{
	float x;
	float y;
	float z;
};

class Matrix4 {
public:
	// �sx��
	float m[4][4];

	Matrix4();
	// �������w�肵�Ă̐���
	Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);
	//�P�ʍs��ŏ�����
	void Initialize();

	//���[���h���W�n
	//�����ŃX�P�[�����v�Z
	void scale(float x, float y, float z);
	//�����ŉ�]���v�Z
	void rotation(float x,float y,float z);
	//�����ŕ��s�ړ����v�Z
	void move(float x, float y, float z);

	//�ˉe�ϊ� ��p �A�X�y�N�g�� �O�[ ���[
	void Perspective(float angle,float aspect,float znear,float zfar);

	//�r���[�ϊ�
	void View(Vector3 eye, Vector3 target, Vector3 up);

	//Matrix4���m�̊|���Z
	void matrixUpdate(Matrix4 m2);
};

//vector3�ƍs��̊|���Z
void worldMatrixScale(Vector3* vector_, Matrix4* Matrix_);
void worldMatrixMove(Vector3* vector_, Matrix4* Matrix_);
void worldMatrixRoll(Vector3* vector_, Matrix4* Matrix_);
//���ׂĂ̊|���Z���܂Ƃ߂��֐�
void vecWorldTransform(Vector3* vector_, Matrix4* Matrix_);

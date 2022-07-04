#include "matrix.h"
#include "math.h"

Matrix4::Matrix4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
	m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
	m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
	m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
	m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
}

void Matrix4::Initialize()
{
	m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
	m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
	m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
	m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 0;

}

void Matrix4::scale(float x, float y, float z)
{
	Matrix4 matScale(
		x, 0.0f, 0.0f, 0.0f,
		0.0f,y, 0.0f, 0.0f,
		0.0f, 0.0f, z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	matrixUpdate(matScale);
}

void Matrix4::rotation(float x, float y, float z)
{
	Matrix4 matRotX(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(x), sin(x), 0.0f,
		0.0f, -sin(x), cos(x), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Matrix4 matRotY(
		cos(y), 0.0f, -sin(y), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin(y), 0.0f, cos(y), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Matrix4 matRotZ(
		cos(z), sin(z), 0.0f, 0.0f,
		-sin(z), cos(z), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	matrixUpdate(matRotZ);
	matrixUpdate(matRotX);
	matrixUpdate(matRotY);
}

void Matrix4::move(float x, float y, float z)
{
	Matrix4 matTrans(
		1.0f, 0.0f, 0.0f, 0,
		0.0f, 1.0f, 0.0f, 0,
		0.0f, 0.0f, 1.0f, 0,
		x, y, z, 1.0f
	);
	matrixUpdate(matTrans);
}

void Matrix4::Perspective(float angle, float aspect, float znear, float zfar)
{
	Matrix4 matPerspective(
		tan(angle/2)/aspect, 0.0f, 0.0f, 0,
		0.0f, tan(angle/2), 0.0f, 0,
		0.0f, 0.0f, -(zfar + znear) / (zfar - znear), 0,
		0.0f, 0.0f,-1.0f, 1.0f
	);
	matrixUpdate(matPerspective);
}

void Matrix4::View(Vector3 eye, Vector3 target, Vector3 up)
{
}

void Matrix4::matrixUpdate(Matrix4 m2)
{
	//ílÇì¸ÇÍÇƒÇ®Ç≠ÇÊÇ§ÇÃçsóÒÇópà”
	Matrix4 m3;
	m3.Initialize();

	//ä|ÇØéZ
	//ç∂Ç©ÇÁ1óÒñ⁄
	m3.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
	m3.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
	m3.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
	m3.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0]	+ m[3][3] * m2.m[3][0];
	//ç∂Ç©ÇÁ2óÒñ⁄
	m3.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
	m3.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
	m3.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
	m3.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1]	+ m[3][3] * m2.m[3][1];
	//ç∂Ç©ÇÁ3óÒñ⁄
	m3.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
	m3.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
	m3.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
	m3.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
	//ç∂Ç©ÇÁ4óÒñ⁄
	m3.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];
	m3.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];
	m3.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];
	m3.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

	//ë„ì¸
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = m3.m[i][j];
		}
	}
}	

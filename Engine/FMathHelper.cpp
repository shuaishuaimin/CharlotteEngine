#include "stdafx.h"
#include <float.h>
#include <cmath>
#include "FMathHelper.h"


using namespace DirectX;

const float FMathHelper::Infinity = FLT_MAX;
const float FMathHelper::Pi = 3.1415926535f;

float FMathHelper::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * Pi; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + Pi; // in [0, 2*pi).

	return theta;
}

XMVECTOR FMathHelper::RandUnitVec3()
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(FMathHelper::RandF(-1.0f, 1.0f), FMathHelper::RandF(-1.0f, 1.0f), FMathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		return XMVector3Normalize(v);
	}
}

XMVECTOR FMathHelper::RandHemisphereUnitVec3(XMVECTOR n)
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(FMathHelper::RandF(-1.0f, 1.0f), FMathHelper::RandF(-1.0f, 1.0f), FMathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		// Ignore points in the bottom hemisphere.
		if (XMVector3Less(XMVector3Dot(n, v), Zero))
			continue;

		return XMVector3Normalize(v);
	}
}

DirectX::XMVECTOR FMathHelper::VectorMultipyMatrix(const DirectX::XMVECTOR& Vector, const DirectX::XMMATRIX& Matrix)
{
	XMVECTOR ResultVector;
	XMFLOAT4 VectorFloat;
	XMStoreFloat4(&VectorFloat, Vector);

	XMFLOAT4X4 MatrixFloat;
	XMStoreFloat4x4(&MatrixFloat, Matrix);

	XMFLOAT4 ResultFloat;
	ResultFloat.x = VectorFloat.x * MatrixFloat._11 + VectorFloat.y * MatrixFloat._21
		+ VectorFloat.z * MatrixFloat._31 + VectorFloat.w * MatrixFloat._41;

	ResultFloat.y = VectorFloat.x * MatrixFloat._12 + VectorFloat.y * MatrixFloat._22
		+ VectorFloat.z * MatrixFloat._32 + VectorFloat.w * MatrixFloat._42;

	ResultFloat.z = VectorFloat.x * MatrixFloat._13 + VectorFloat.y * MatrixFloat._23
		+ VectorFloat.z * MatrixFloat._33 + VectorFloat.w * MatrixFloat._43;

	ResultFloat.w = VectorFloat.x * MatrixFloat._14 + VectorFloat.y * MatrixFloat._24
		+ VectorFloat.z * MatrixFloat._34 + VectorFloat.w * MatrixFloat._44;

	ResultVector = XMLoadFloat4(&ResultFloat);
	return ResultVector;
}


DirectX::XMMATRIX FMathHelper::GetWorldTransMatrix(const Charalotte::FTransform& Transform)
{
	DirectX::XMMATRIX Transport;
 
	DirectX::XMMATRIX ScaleTrans = XMMatrixScaling(Transform.Scale3D.x, Transform.Scale3D.y, Transform.Scale3D.z);
	
	DirectX::XMVECTOR Quater = XMVectorSet(Transform.Rotation.X, Transform.Rotation.Y, Transform.Rotation.Z, Transform.Rotation.W);
	DirectX::XMMATRIX RotateTrans = XMMatrixRotationQuaternion(Quater);

	DirectX::XMMATRIX DisplaymentTrans = XMMatrixTranslation(Transform.Translation.x, Transform.Translation.y, Transform.Translation.z);
	Transport = ScaleTrans * RotateTrans * DisplaymentTrans;

	return Transport;
}

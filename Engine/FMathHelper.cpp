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


glm::mat4 FMathHelper::GetWorldTransMatrix(const Charalotte::FTransform& Transform)
{
	glm::mat4 Transport; 
	glm::mat4 ScaleTrans = glm::mat4(1.0f);
	ScaleTrans = glm::scale(ScaleTrans, glm::vec3(Transform.Scale3D.x, Transform.Scale3D.y, Transform.Scale3D.z));
	
	glm::qua<float> Quater = glm::qua<float>(Transform.Rotation.W, Transform.Rotation.X, Transform.Rotation.Y, Transform.Rotation.Z);
	glm::mat4 RotateTrans = glm::mat4_cast(Quater);

	glm::mat4 DisplaymentTrans = glm::mat4(1.0f);
	DisplaymentTrans = glm::translate(DisplaymentTrans, glm::vec3(Transform.Translation.x, Transform.Translation.y, Transform.Translation.z));

	Transport = DisplaymentTrans * RotateTrans * ScaleTrans;

	return Transport;
}

glm::mat4 FMathHelper::GetRotateMatrix(float Pitch, 
				float Yaw, float Roll, const glm::vec4& Target, const glm::vec4& Up, const glm::vec4& Location)
{
	glm::mat4 ResultMatrix = glm::mat4(1.0f);

	glm::mat4 LocationTransBackZero = glm::mat4(1.0f);
	LocationTransBackZero = glm::translate(LocationTransBackZero, glm::vec3(Location.x * -1.0f, Location.y * -1.0f, Location.z * -1.0f));
	glm::mat4 LocationTransBackToStart = glm::mat4(1.0f);
	LocationTransBackToStart = glm::translate(LocationTransBackZero, glm::vec3(Location.x, Location.y, Location.z));

	glm::vec3 UpXLocation = glm::cross(glm::vec3(Up.x, Up.y, Up.z), glm::vec3(Location.x, Location.y, Location.z));

	glm::mat4 YawTransForm = glm::mat4(1.0f);
	if (Yaw != 0.0f)
	{
		YawTransForm = glm::rotate(YawTransForm, glm::radians(Yaw), glm::vec3(Up.x, Up.y, Up.z));
	}
	
	glm::mat4 RollTransForm = glm::mat4(1.0f);
	if (Roll != 0.0f)
	{
		RollTransForm = glm::rotate(RollTransForm, glm::radians(Roll), glm::vec3(Target.x, Target.y, Target.z));
	}
	
	glm::mat4 PitchTransForm = glm::mat4(1.0f);
	if (Pitch != 0.0f)
	{
		PitchTransForm = glm::rotate(PitchTransForm, glm::radians(Pitch), glm::vec3(UpXLocation.x, UpXLocation.y, UpXLocation.z));
	}
	
	ResultMatrix = glm::transpose(LocationTransBackToStart) * YawTransForm * 
							RollTransForm * PitchTransForm * glm::transpose(LocationTransBackZero);

	return ResultMatrix;
}

glm::vec4 FMathHelper::Vec4MultipyMat(const glm::vec4& vector, const glm::mat4& Matrix)
{
	glm::vec4 ResultVector = glm::vec4(1.0f);
	ResultVector.x = vector.x * Matrix[0][0] + vector.y * Matrix[0][1] + vector.z * Matrix[0][2] + vector.w * Matrix[0][3];
	ResultVector.y = vector.x * Matrix[1][0] + vector.y * Matrix[1][1] + vector.z * Matrix[1][2] + vector.w * Matrix[1][3];
	ResultVector.z = vector.x * Matrix[2][0] + vector.y * Matrix[2][1] + vector.z * Matrix[2][2] + vector.w * Matrix[2][3];
	ResultVector.w = vector.x * Matrix[3][0] + vector.y * Matrix[3][1] + vector.z * Matrix[3][2] + vector.w * Matrix[3][3];

	return ResultVector;
}

glm::mat4 FMathHelper::GetRotateMatrix(const glm::vec4& Qua)
{
	glm::qua<float> Quater = glm::qua<float>(Qua.w, Qua.x, Qua.y, Qua.z);
	return glm::mat4_cast(Quater);
}

glm::vec4 FMathHelper::Vector3TransformCoord(glm::vec3 V, glm::mat4 M)
{
	glm::vec4 Z(V.z, V.z, V.z, V.z);
	glm::vec4 Y(V.y, V.y, V.y, V.y);
	glm::vec4 X(V.x, V.x, V.x, V.x);

	glm::vec4 Result = Z * M[2] + M[3];
	Result = Y * M[1] + Result;
	Result = X * M[0] + Result;
	glm::vec4 W(Result.w, Result.w, Result.w, Result.w);

	return Result / W;
}

glm::mat4 FMathHelper::GetScaleMatrix(const Charalotte::FTransform& Transform)
{
	glm::mat4 ScaleTrans = glm::mat4(1.0f);
	ScaleTrans = glm::scale(ScaleTrans, glm::vec3(Transform.Scale3D.x, Transform.Scale3D.y, Transform.Scale3D.z));
	return ScaleTrans;
}
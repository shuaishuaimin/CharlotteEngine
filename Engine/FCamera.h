#pragma once

#include "FWinRender.h"
#include "WinBaseData.h"
#include "FMathHelper.h"

class FCamera
{
public:
	FCamera(const Charalotte::CameraData& Data);
	~FCamera();

	void GetVPTransform(glm::mat4& Matrix);
	void TransformCamera(const Charalotte::CameraTransform& Transform);
	void ChangeAspectRatio(float NewAspectRatio);
	void ChangeFovAngle(float NewFovAngle);
	void AddFovAngle(float AngleForAdd);

	void GetCameraData(Charalotte::CameraData& Data);
	void BackCameraLocation(const glm::vec4& CameraLocation, const glm::vec4& Target, const glm::vec4& Up);

	static Charalotte::FPoint2D LastCameraMousePos;
protected:
	void CalcVPMatrix();
	void RegisterCameraInput();

private:
	Charalotte::VPTransformData VPTransform;
	Charalotte::CameraData MainCameraData;

	glm::mat4 mView = FMathHelper::Identity4x4();
	glm::mat4 mProj = FMathHelper::Identity4x4();
	glm::mat4 mMVP = FMathHelper::Identity4x4();

	float Sensitivity;
};


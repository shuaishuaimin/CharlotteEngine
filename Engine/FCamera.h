#pragma once

#include "FWinsApp.h"
#include "BaseData.h"
#include "FMathHelper.h"

class FCamera
{
public:
	FCamera(const Charalotte::CameraData& Data);
	~FCamera();

	void GetVPTransform(DirectX::XMMATRIX& Matrix);
	void TransformCamera(const Charalotte::CameraTransform& Transform);
	void ChangeAspectRatio(float NewAspectRatio);
	void ChangeFovAngle(float NewFovAngle);
	void AddFovAngle(float AngleForAdd);

	void GetCameraData(Charalotte::CameraData& Data);

protected:
	void CalcVPMatrix();

private:
	DirectX::XMMATRIX MVPMatrix;
	Charalotte::VPTransformData VPTransform;
	Charalotte::CameraData MainCameraData;

	DirectX::XMFLOAT4X4 mView = FMathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = FMathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mMVP = FMathHelper::Identity4x4();

	float Sensitivity;
};
#pragma once

#include "FWinsApp.h"
#include "BaseData.h"
#include "FMathHelper.h"

class Camera
{
public:
	Camera(const Charalotte::CameraData& Data);
	~Camera();

	void GetMVPTransform(DirectX::XMMATRIX& Matrix);
	void TransformCamera(const Charalotte::CameraTransform& Transform, DirectX::XMMATRIX& NewMvpMatrix);
	void ChangeAspectRatio(float NewAspectRatio);
	void ChangeFovAngle(float NewFovAngle);
	void AddFovAngle(float AngleForAdd);

	void GetCameraData(Charalotte::CameraData& Data);

protected:
	void CalcMVPMatrix();

private:
	DirectX::XMMATRIX MVPMatrix;
	Charalotte::MVPTransformData cameraTransform;
	Charalotte::CameraData MainCameraData;

	DirectX::XMFLOAT4X4 mWorld = FMathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mView = FMathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = FMathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mMVP = FMathHelper::Identity4x4();

	float Sensitivity;
};
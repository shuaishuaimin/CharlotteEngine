#include "stdafx.h"
#include "FCamera.h"


using namespace DirectX;

FCamera::FCamera(const Charalotte::CameraData& Data) : MainCameraData(Data), Sensitivity(0.25)
{
	VPTransform.ViewTransform = XMLoadFloat4x4(&mView);
	VPTransform.ProjectionTransform = XMLoadFloat4x4(&mProj);
	MVPMatrix = XMLoadFloat4x4(&mMVP);
	CalcVPMatrix();
}
FCamera::~FCamera()
{

}

void FCamera::GetVPTransform(DirectX::XMMATRIX& Matrix)
{
	Matrix = MVPMatrix;
}

// Can not use
void FCamera::TransformCamera(const Charalotte::CameraTransform& Transform)
{
	XMMATRIX DisplacementMatrix = XMMatrixTranslation(Transform.Translation.x, Transform.Translation.y, Transform.Translation.z);
	MainCameraData.Location = FMathHelper::VectorMultipyMatrix(MainCameraData.Location, DisplacementMatrix);

	XMMATRIX RotateMatrix = XMMatrixRotationRollPitchYaw(Transform.pitch, Transform.yaw, Transform.row);
	XMFLOAT4 LocationFloat;
	XMVECTOR Location = MainCameraData.Location;
	XMStoreFloat4(&LocationFloat, Location);
	XMMATRIX BackToZeroMatrix = XMMatrixTranslation(LocationFloat.x * -1.0f, LocationFloat.y * -1.0f, LocationFloat.z * -1.0f);
	XMMATRIX BackToStartMatrix = XMMatrixTranslation(LocationFloat.x, LocationFloat.y, LocationFloat.z);
	
	XMMATRIX FinalRotateMatrix = BackToZeroMatrix * RotateMatrix * BackToStartMatrix;

	MainCameraData.Target = FMathHelper::VectorMultipyMatrix(MainCameraData.Target, FinalRotateMatrix);
	//MainCameraData.Up = FMathHelper::VectorMultipyMatrix(MainCameraData.Up, FinalRotateMatrix);
	CalcVPMatrix();
}

void FCamera::ChangeAspectRatio(float NewAspectRatio)
{
	MainCameraData.AspectRatio = NewAspectRatio;
	CalcVPMatrix();
}

void FCamera::ChangeFovAngle(float NewFovAngle)
{
	MainCameraData.FovAngleY = NewFovAngle;
	CalcVPMatrix();
}

void FCamera::AddFovAngle(float AngleForAdd)
{
	MainCameraData.FovAngleY += AngleForAdd;
	CalcVPMatrix();
}

void FCamera::GetCameraData(Charalotte::CameraData& Data)
{
	Data = MainCameraData;
	CalcVPMatrix();
}

void FCamera::CalcVPMatrix()
{
	XMMATRIX view = XMMatrixLookAtLH(MainCameraData.Location,
		MainCameraData.Location + MainCameraData.Target, MainCameraData.Up);
	XMStoreFloat4x4(&mView, view);
	VPTransform.ViewTransform = view;

	XMMATRIX proj;
	proj = XMMatrixPerspectiveFovLH(MainCameraData.FovAngleY,
		MainCameraData.AspectRatio, MainCameraData.Near, MainCameraData.Far);
	VPTransform.ProjectionTransform = proj;
	XMStoreFloat4x4(&mProj, proj);

	VPTransform.VPMatrix = view * proj;
	MVPMatrix =view * proj;
}

void FCamera::BackCameraLocation(const DirectX::XMVECTOR& CameraLocation, const DirectX::XMVECTOR& Target, const DirectX::XMVECTOR& Up)
{
	MainCameraData.Location = CameraLocation;
	MainCameraData.Target = Target;
	MainCameraData.Up = Up;
	CalcVPMatrix();
}
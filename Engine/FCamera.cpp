#include "stdafx.h"
#include "FCamera.h"


using namespace DirectX;

Camera::Camera(const Charalotte::CameraData& Data) : MainCameraData(Data), Sensitivity(0.25)
{
	cameraTransform.WorldTransform = XMLoadFloat4x4(&mWorld);
	cameraTransform.ViewTransform = XMLoadFloat4x4(&mView);
	cameraTransform.ProjectionTransform = XMLoadFloat4x4(&mProj);
	MVPMatrix = XMLoadFloat4x4(&mMVP);
	CalcMVPMatrix();
}
Camera::~Camera()
{

}

void Camera::GetMVPTransform(DirectX::XMMATRIX& Matrix)
{
	Matrix = MVPMatrix;
}

void Camera::TransformCamera(const Charalotte::CameraTransform& Transform, DirectX::XMMATRIX& NewMvpMatrix)
{
	XMMATRIX DisplacementMatrix = XMMatrixTranslation(Transform.Translation.x, Transform.Translation.y, Transform.Translation.z);
	MainCameraData.Location = FMathHelper::VectorMultipyMatrix(MainCameraData.Location, XMMatrixTranspose(DisplacementMatrix));
	XMMATRIX RotateMatrix = XMMatrixRotationRollPitchYaw(Transform.pitch, Transform.yaw, Transform.row);

	XMFLOAT4 LocationFloat;
	XMStoreFloat4(&LocationFloat, MainCameraData.Location);
	XMMATRIX TransToZero = XMMatrixTranslation(-1.0f * LocationFloat.x, -1.0f * LocationFloat.y, -1.0f * LocationFloat.z);
	XMMATRIX TransZeroToBack = XMMatrixTranslation(LocationFloat.x, LocationFloat.y, LocationFloat.z);
}

void Camera::ChangeAspectRatio(float NewAspectRatio)
{
	MainCameraData.AspectRatio = NewAspectRatio;
	CalcMVPMatrix();
}

void Camera::ChangeFovAngle(float NewFovAngle)
{
	MainCameraData.FovAngleY = NewFovAngle;
	CalcMVPMatrix();
}

void Camera::AddFovAngle(float AngleForAdd)
{
	MainCameraData.FovAngleY += AngleForAdd;
	CalcMVPMatrix();
}

void Camera::GetCameraData(Charalotte::CameraData& Data)
{
	Data = MainCameraData;
	CalcMVPMatrix();
}

void Camera::CalcMVPMatrix()
{
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMMatrixLookAtLH(MainCameraData.Location,
		MainCameraData.Target, MainCameraData.Up);
	XMStoreFloat4x4(&mView, view);
	cameraTransform.ViewTransform = view;

	XMMATRIX proj;
	proj = XMMatrixPerspectiveFovLH(MainCameraData.FovAngleY,
		MainCameraData.AspectRatio, MainCameraData.Near, MainCameraData.Far);
	cameraTransform.ProjectionTransform = proj;
	XMStoreFloat4x4(&mProj, proj);

	cameraTransform.MVPMatrix = world * view * proj;
}


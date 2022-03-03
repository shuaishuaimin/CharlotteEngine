#include "stdafx.h"
#include "FCamera.h"

FCamera::FCamera(const Charalotte::CameraData& Data) : MainCameraData(Data), Sensitivity(0.25)
{
	VPTransform.ViewTransform = glm::mat4(1.0f);
	VPTransform.ProjectionTransform = glm::mat4(1.0f);;
	CalcVPMatrix();
}
FCamera::~FCamera()
{

}

void FCamera::GetVPTransform(glm::mat4& Matrix)
{
	Matrix = VPTransform.VPMatrix;
}

// Can not use
void FCamera::TransformCamera(const Charalotte::CameraTransform& Transform)
{
	auto Result = MainCameraData.Target;
	glm::mat4 DisplacementMatrix = glm::mat4(1.0f);
	DisplacementMatrix = glm::translate(DisplacementMatrix, glm::vec3(Transform.Translation.x, Transform.Translation.y, Transform.Translation.z));
	MainCameraData.Location = FMathHelper::Vec4MultipyMat(MainCameraData.Location, glm::transpose(DisplacementMatrix));

	glm::mat4 RotateMatrix = FMathHelper::GetRotateMatrix(Transform.pitch, Transform.row, Transform.yaw, 
								MainCameraData.Target, MainCameraData.Up, MainCameraData.Location);

	MainCameraData.Target = FMathHelper::Vec4MultipyMat(MainCameraData.Target, glm::transpose(RotateMatrix));
	MainCameraData.Up = FMathHelper::Vec4MultipyMat(MainCameraData.Up, glm::transpose(RotateMatrix));
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
}

void FCamera::CalcVPMatrix()
{
	glm::vec3 Location3 = MainCameraData.Location;
	glm::vec3 Center3 = MainCameraData.Target;
	glm::vec3 Up3 = MainCameraData.Up;
	glm::mat4 view = glm::lookAtLH(Location3, Location3 + Center3, Up3);

	VPTransform.ViewTransform = view;

	glm::mat4 proj;
	proj = glm::perspectiveFovLH(MainCameraData.FovAngleY,
		MainCameraData.Width, MainCameraData.Height,MainCameraData.Near, MainCameraData.Far);
	VPTransform.ProjectionTransform = proj;

	VPTransform.VPMatrix = proj * view;
}

void FCamera::BackCameraLocation(const glm::vec4& CameraLocation, const glm::vec4& Target, const glm::vec4& Up)
{
	MainCameraData.Location = CameraLocation;
	MainCameraData.Target = Target;
	MainCameraData.Up = Up;
	CalcVPMatrix();
}
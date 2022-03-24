#include "stdafx.h"
#include "FCamera.h"
#include "FScene.h"
#include "CharlotteEngine.h"
#include "DXPrimitives.h"

#if PLATFORM_WINDOWS
#include "FWinEventRegisterSystem.h"
#endif

FCamera::FCamera(const Charalotte::CameraData& Data) : MainCameraData(Data), Sensitivity(0.25)
{
	VPTransform.ViewTransform = glm::mat4(1.0f);
	VPTransform.ProjectionTransform = glm::mat4(1.0f);;
	CalcVPMatrix();
	RegisterObjectInput();
	
}
FCamera::FCamera() : Sensitivity(1.5)
{
	Charalotte::CameraData DefaultCameraData;
	DefaultCameraData.Near = 1.0f;
	DefaultCameraData.Far = 20000.0f;
	DefaultCameraData.FovAngleY = 0.25f * FMathHelper::Pi;
	DefaultCameraData.Width = 1980;
	DefaultCameraData.Height = 1280;
	DefaultCameraData.AspectRatio = static_cast<float>(DefaultCameraData.Width / DefaultCameraData.Height);
	DefaultCameraData.Location = glm::vec4(-5000.0f, 0.0f, 0.0f, 1.0f);
	DefaultCameraData.Target = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	DefaultCameraData.Up = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	MainCameraData = DefaultCameraData;
	VPTransform.ViewTransform = glm::mat4(1.0f);
	VPTransform.ProjectionTransform = glm::mat4(1.0f);;
	CalcVPMatrix();
	RegisterObjectInput();
	CharalotteEngine::GetInstance().GetWindowPtr()->SetCameraSen(this->Sensitivity);
}


Charalotte::FPoint2D FCamera::LastCameraMousePos = { 0.0f, 0.0f };
FCamera::~FCamera()
{

}

void FCamera::GetVPTransform(glm::mat4& Matrix)
{
	Matrix = VPTransform.VPMatrix;
}

// Can not use
void FCamera::TransformObject(const Charalotte::CameraTransform& Transform)
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

void FCamera::RegisterObjectInput()
{
	//float KeySe = 0.3f;
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('A', [this]() {
		auto Trans = FScene::GetInstance().GetCameraTrans();
		float KeySe = this->Sensitivity;
		Trans.Translation.y -= KeySe;
		FScene::GetInstance().GetCamera()->TransformObject(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('D', [this]() {
		auto Trans = FScene::GetInstance().GetCameraTrans();
		float KeySe = 1.5f;
		Trans.Translation.y += this->Sensitivity;
		FScene::GetInstance().GetCamera()->TransformObject(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('W', [this]() {
		auto Trans = FScene::GetInstance().GetCameraTrans();
		float KeySe = 1.5f;
		Trans.Translation.x += this->Sensitivity;
		FScene::GetInstance().GetCamera()->TransformObject(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('S', [this]() {
		auto Trans = FScene::GetInstance().GetCameraTrans();
		float KeySe = 1.5f;
		Trans.Translation.x -= this->Sensitivity;
		FScene::GetInstance().GetCamera()->TransformObject(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('Q', [this]() {
		auto Trans = FScene::GetInstance().GetCameraTrans();
		float KeySe = 1.5f;
		Trans.Translation.z -= this->Sensitivity;
		FScene::GetInstance().GetCamera()->TransformObject(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('E', [this]() {
		auto Trans = FScene::GetInstance().GetCameraTrans();
		float KeySe = 1.5f;
		Trans.Translation.z += this->Sensitivity;
		FScene::GetInstance().GetCamera()->TransformObject(Trans);
		FScene::GetInstance().InitCameraTrans();
		});
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('O', []() {
		glm::vec4 Location = glm::vec4(-5000.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 Target = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 Up = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		FScene::GetInstance().GetCamera()->BackCameraLocation(Location, Target, Up);
		});
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('R', [this]() {
		this->Sensitivity += 0.1f;
		CharalotteEngine::GetInstance().GetWindowPtr()->SetCameraSen(this->Sensitivity);
		});
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('T', [this]() {
		if (this->Sensitivity > 0.0f)
		{
			this->Sensitivity -= 0.1f;
		}
		else
		{
			this->Sensitivity = 0.0f;
		}
		CharalotteEngine::GetInstance().GetWindowPtr()->SetCameraSen(this->Sensitivity);
		});

	FWinEventRegisterSystem::GetInstance().RegisterWindowsInput
	(Charalotte::WindowsAction::MouseDown, [](const Charalotte::WindowsInputData& Mouse) {
		FCamera::LastCameraMousePos.x = static_cast<float>(Mouse.x);
		FCamera::LastCameraMousePos.y = static_cast<float>(Mouse.y);
		});

	FWinEventRegisterSystem::GetInstance().RegisterWindowsInput
	(Charalotte::WindowsAction::MouseUp, [](const Charalotte::WindowsInputData& Mouse) {
		ReleaseCapture();
		});
#if PLATFORM_WINDOWS
	FWinEventRegisterSystem::GetInstance().RegisterWindowsInput
	(Charalotte::WindowsAction::Move, [](const Charalotte::WindowsInputData& Mouse) {
		float MouseSe = 0.03f;
		if ((Mouse.btnState & MK_LBUTTON) != 0)
		{
			// make each pixel correspond to a quarter of a degree
			float dx = -MouseSe * static_cast<float>(Mouse.x - FCamera::LastCameraMousePos.x);
			float dy = -MouseSe * static_cast<float>(Mouse.y - FCamera::LastCameraMousePos.y);
			auto& CameraTrans = FScene::GetInstance().GetCameraTrans();
			CameraTrans.row += (dx + dy);
			FScene::GetInstance().GetCamera()->TransformObject(CameraTrans);
			FScene::GetInstance().InitCameraTrans();
		}
		else if ((Mouse.btnState & MK_RBUTTON) != 0)
		{

			// Make each pixel correspond to 0.005 unit in the scene
			float dx = MouseSe * static_cast<float>(Mouse.x - FCamera::LastCameraMousePos.x);
			float dy = MouseSe * static_cast<float>(Mouse.y - FCamera::LastCameraMousePos.y);
			auto& CameraTrans = FScene::GetInstance().GetCameraTrans();
			CameraTrans.pitch += (dx + dy);
			FScene::GetInstance().GetCamera()->TransformObject(CameraTrans);
			FScene::GetInstance().InitCameraTrans();
		}

		FCamera::LastCameraMousePos.x = static_cast<float>(Mouse.x);
		FCamera::LastCameraMousePos.y = static_cast<float>(Mouse.y);
		});
#endif
}
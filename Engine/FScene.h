#pragma once
#include <memory>
#include <vector>
#include "Singleton.h"
#include "FCamera.h"
#include "BaseData.h"

class FScene : public Singleton<FScene>
{
public:
	FScene(){
		Charalotte::CameraData CameraData;
		CameraData.Near = 1.0f;
		CameraData.Far = 20000.0f;
		CameraData.FovAngleY = 0.25f * FMathHelper::Pi;
		CameraData.Width = 1980;
		CameraData.Height = 1280;
		CameraData.AspectRatio = static_cast<float>(CameraData.Width / CameraData.Height);
		CameraData.Location = glm::vec4(-5000.0f, 0.0f, 0.0f, 1.0f);
		CameraData.Target = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		CameraData.Up = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		MainCamera = std::make_shared<FCamera>(CameraData);
		Cameras.push_back(MainCamera);
		CameraTrans = Charalotte::CameraTransform();
		DefaultCameraTrans = Charalotte::CameraTransform();
	}
	~FScene(){}

	FCamera* GetCamera()
	{
		return MainCamera.get();
	}

	Charalotte::CameraTransform& GetCameraTrans()
	{
		return CameraTrans;
	}

	void InitCameraTrans()
	{
		CameraTrans = DefaultCameraTrans;
	}
private:
	std::vector<std::shared_ptr<FCamera>> Cameras;
	std::shared_ptr<FCamera> MainCamera;
	Charalotte::CameraTransform CameraTrans;
	Charalotte::CameraTransform DefaultCameraTrans;
};
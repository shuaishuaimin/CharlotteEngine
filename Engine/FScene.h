#pragma once
#include <memory>
#include <vector>
#include "Singleton.h"
#include "FCamera.h"
#include "BaseData.h"
#include "FAssetSystem.h"

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

		GameTimer = std::make_unique<FGameTimer>();
		IsAppPaused = false;
		AssetSystemIns = std::make_unique<FAssetSystem>();
		IsDeviceSucceed = false;
		IsCanResizing = false;
	}
	~FScene(){
		GameTimer = nullptr;
		AssetSystemIns = nullptr;
		MainCamera = nullptr;
	}

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

	FGameTimer* GetTimer()
	{
		return GameTimer.get();
	}

	bool GetIsAppPaused()
	{
		return IsAppPaused;
	}

	void SetIsAppPaused(bool IsPaused)
	{
		IsAppPaused = IsPaused;
	}

	bool GetIsDeviceSucceed()
	{
		return IsDeviceSucceed;
	}

	void SetIsDeviceSucceed(bool IsSucceed)
	{
		IsDeviceSucceed = IsSucceed;
	}

	bool GetIsCanResizing()
	{
		return IsCanResizing;
	}

	void SetIsCanResizing(bool IsCan)
	{
		IsCanResizing = IsCan;
	}

	FAssetSystem* GetAssetSystem()
	{
		return AssetSystemIns.get();
	}
private:
	std::vector<std::shared_ptr<FCamera>> Cameras;
	std::shared_ptr<FCamera> MainCamera;
	Charalotte::CameraTransform CameraTrans;
	Charalotte::CameraTransform DefaultCameraTrans;

	std::unique_ptr<FGameTimer> GameTimer;

	std::unique_ptr<FAssetSystem> AssetSystemIns;
	// connect app and window
	bool IsAppPaused;

	bool IsDeviceSucceed;

	bool IsCanResizing;
};
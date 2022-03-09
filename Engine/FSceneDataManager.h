#pragma once
#include <memory>
#include <vector>
#include <set>
#include "Singleton.h"
#include "FCamera.h"
#include "WinBaseData.h"
#include "FGameTimer.h"
#include "FWinEventRegisterSystem.h"

class FSceneDataManager : public Singleton<FSceneDataManager>
{
public:
	FSceneDataManager(){
		
		MainCamera = std::make_shared<FCamera>();
		Cameras.push_back(MainCamera);
		CameraTrans = Charalotte::CameraTransform();
		DefaultCameraTrans = Charalotte::CameraTransform();

		GameTimer = std::make_unique<FGameTimer>();
		IsAppPaused = false;
		IsDeviceSucceed = false;
		IsCanResizing = false;
	}
	~FSceneDataManager(){
		GameTimer = nullptr;
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

	std::unordered_map<std::string, Charalotte::FActorsInfoForPrint> GetActorInfos()
	{
		return ActorInfors;
	}
	std::unordered_map<std::string, Charalotte::FMeshInfoForPrint> GetMeshInfors()
	{
		return MeshsInfors;
	}
	void LoadMap(const std::string& MapName) {
		Charalotte::FActorsInfoForPrint TempActorInfors;
		FDataProcessor::LoadActors(MapName, TempActorInfors);
		ActorInfors.insert({ MapName, TempActorInfors });
		std::set<std::string> AssetNames;
		for (const auto& ActorInfor : TempActorInfors.ActorsInfo)
		{
			std::string AssetName = ActorInfor.AssetName;
			if (AssetName.size() > 0)
			{
				AssetName.erase(AssetName.size() - 1, 1);
			}
			AssetName += ".dat";
			AssetNames.insert(AssetName);
		}
		for (const auto& AssetName : AssetNames)
		{
			Charalotte::FMeshInfoForPrint MeshInfo;
			FDataProcessor::LoadMesh(AssetName, MeshInfo);
			std::string NameWithout = AssetName;

			if (NameWithout.size() > 4)
			{
				NameWithout.erase(NameWithout.size() - 4, 4);
				MeshsInfors.insert({ NameWithout, MeshInfo });
			}
		}
		FWinEventRegisterSystem::GetInstance().ExecuteMapLoadEvent(MapName);
	}

	Charalotte::FMeshInfoForPrint GetMeshInfoByName(const std::string& MeshName)
	{
		auto MeshInfoIter = MeshsInfors.find(MeshName);
		if (MeshInfoIter != MeshsInfors.end())
		{
			return MeshInfoIter->second;
		}
		return Charalotte::FMeshInfoForPrint();
	}


private:
	std::vector<std::shared_ptr<FCamera>> Cameras;
	std::shared_ptr<FCamera> MainCamera;
	Charalotte::CameraTransform CameraTrans;
	Charalotte::CameraTransform DefaultCameraTrans;

	std::unordered_map<std::string, Charalotte::FActorsInfoForPrint> ActorInfors;
	std::unordered_map<std::string, Charalotte::FMeshInfoForPrint> MeshsInfors;

	std::unique_ptr<FGameTimer> GameTimer;
	// connect app and window
	bool IsAppPaused;

	bool IsDeviceSucceed;

	bool IsCanResizing;
};
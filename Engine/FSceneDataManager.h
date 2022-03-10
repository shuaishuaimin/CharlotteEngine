#pragma once
#include <memory>
#include <vector>
#include <set>
#include "Singleton.h"
#include "FCamera.h"
#include "WinBaseData.h"
#include "FGameTimer.h"
#include "FWinEventRegisterSystem.h"
#include "FWinSceneAsset.h"

class FSceneDataManager : public Singleton<FSceneDataManager>
{
public:
	FSceneDataManager();
	~FSceneDataManager();

	FCamera* GetCamera();
	
	Charalotte::CameraTransform& GetCameraTrans();
	
	void InitCameraTrans();
	
	FGameTimer* GetTimer();

	bool GetIsAppPaused();

	void SetIsAppPaused(bool IsPaused);

	bool GetIsDeviceSucceed();

	void SetIsDeviceSucceed(bool IsSucceed);

	bool GetIsCanResizing();

	void SetIsCanResizing(bool IsCan);

	void Update();

	std::unordered_map<std::string, Charalotte::FActorsInfoForPrint> GetActorInfos();

	std::unordered_map<std::string, Charalotte::FMeshInfoForPrint> GetMeshInfors();

	void LoadMap(const std::string& MapName);

	Charalotte::FMeshInfoForPrint GetMeshInfoByName(const std::string& MeshName);

	std::unordered_map<std::string, std::vector<std::shared_ptr<Charalotte::FActorAsset>>>& GetActorDictionary();

	std::vector<std::shared_ptr<Charalotte::FActorAsset>>& GetSceneActorByName(const std::string& MapName);
protected:

	void CalcVerticesAndIndices(const std::string& GeometryName, const Charalotte::FTransform& Transform);

	void BuilMeshAsset(const std::string& MapName);

	void BuildActors(const std::string& MapName);

private:
	std::vector<std::shared_ptr<FCamera>> Cameras;
	std::shared_ptr<FCamera> MainCamera;
	Charalotte::CameraTransform CameraTrans;
	Charalotte::CameraTransform DefaultCameraTrans;

	// save the actor information without render
	std::unordered_map<std::string, Charalotte::FActorsInfoForPrint> ActorInfors;

	// save the actor information have been rendered;
	std::unordered_map<std::string, std::vector<std::shared_ptr<Charalotte::FActorAsset>>> ActorDir;

	std::vector<std::shared_ptr<Charalotte::FActorAsset>> EmptyActorVec;
	// save the mesh information without render, the real data is in win scene asset
	std::unordered_map<std::string, Charalotte::FMeshInfoForPrint> MeshsInfors;

	std::unique_ptr<FGameTimer> GameTimer;
	// connect app and window
	bool IsAppPaused;

	bool IsDeviceSucceed;

	bool IsCanResizing;

	std::string NowMapName;
};
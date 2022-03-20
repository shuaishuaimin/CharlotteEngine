#pragma once
#include <memory>
#include <vector>
#include <set>
#include "Singleton.h"
#include "FCamera.h"
#include "DXPrimitives.h"
#include "FGameTimer.h"
#include "FWinEventRegisterSystem.h"
#include "FDXResources.h"

class FScene : public Singleton<FScene>
{
public:
	FScene();
	~FScene();

	FCamera* GetCamera();
	
	Charalotte::CameraTransform& GetCameraTrans();
	
	void InitCameraTrans();

	void Update();

	std::unordered_map<std::string, Charalotte::FActorPrimitive>& GetActorInfos();

	void LoadMap(const std::string& MapName);

	std::unordered_map<std::string, std::vector<std::shared_ptr<Charalotte::FDXActorPrimitive>>>& GetActorDictionary();

	std::vector<std::shared_ptr<Charalotte::FDXActorPrimitive>>& GetSceneActorByName(const std::string& MapName);

private:
	std::vector<std::shared_ptr<FCamera>> Cameras;
	std::shared_ptr<FCamera> MainCamera;
	Charalotte::CameraTransform CameraTrans;
	Charalotte::CameraTransform DefaultCameraTrans;

	// save the actor information without render
	std::unordered_map<std::string, Charalotte::FActorPrimitive> ActorInfors;

	// save the actor information have been rendered;
	std::unordered_map<std::string, std::vector<std::shared_ptr<Charalotte::FDXActorPrimitive>>> ActorDir;

	std::vector<std::shared_ptr<Charalotte::FDXActorPrimitive>> EmptyActorVec;

	std::string NowMapName;

	std::unordered_map<std::string, int32_t> MeshPrimitiveTimes;
};
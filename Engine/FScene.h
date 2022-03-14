#pragma once
#include <memory>
#include <vector>
#include <set>
#include "Singleton.h"
#include "FCamera.h"
#include "WinFrameResource.h"
#include "FGameTimer.h"
#include "FWinEventRegisterSystem.h"
#include "FDXRenderMeshDataBuffer.h"

class FScene : public Singleton<FScene>
{
public:
	FScene();
	~FScene();

	FCamera* GetCamera();
	
	Charalotte::CameraTransform& GetCameraTrans();
	
	void InitCameraTrans();

	void Update();

	std::unordered_map<std::string, Charalotte::FActorsInfoForPrint> GetActorInfos();


	void LoadMap(const std::string& MapName);


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

	std::string NowMapName;
};
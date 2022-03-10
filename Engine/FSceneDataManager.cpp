#include "stdafx.h"
#include "FSceneDataManager.h"

FSceneDataManager::FSceneDataManager() {

	MainCamera = std::make_shared<FCamera>();
	Cameras.push_back(MainCamera);
	CameraTrans = Charalotte::CameraTransform();
	DefaultCameraTrans = Charalotte::CameraTransform();

	GameTimer = std::make_unique<FGameTimer>();
	ActorDir = {};
	EmptyActorVec = {};
	NowMapName = "";
	IsAppPaused = false;
	IsDeviceSucceed = false;
	IsCanResizing = false;
}
FSceneDataManager::~FSceneDataManager() {
	GameTimer = nullptr;
	MainCamera = nullptr;
	for (auto& ActorVector : ActorDir)
	{
		for (auto& ActorToClear : ActorVector.second)
		{
			ActorToClear = nullptr;
		}
	}
	ActorDir.clear();
}

FCamera* FSceneDataManager::GetCamera()
{
	return MainCamera.get();
}

Charalotte::CameraTransform& FSceneDataManager::GetCameraTrans()
{
	return CameraTrans;
}

void FSceneDataManager::InitCameraTrans()
{
	CameraTrans = DefaultCameraTrans;
}

FGameTimer* FSceneDataManager::GetTimer()
{
	return GameTimer.get();
}

bool FSceneDataManager::GetIsAppPaused()
{
	return IsAppPaused;
}

void FSceneDataManager::SetIsAppPaused(bool IsPaused)
{
	IsAppPaused = IsPaused;
}

bool FSceneDataManager::GetIsDeviceSucceed()
{
	return IsDeviceSucceed;
}

void FSceneDataManager::SetIsDeviceSucceed(bool IsSucceed)
{
	IsDeviceSucceed = IsSucceed;
}

bool FSceneDataManager::GetIsCanResizing()
{
	return IsCanResizing;
}

void FSceneDataManager::SetIsCanResizing(bool IsCan)
{
	IsCanResizing = IsCan;
}

std::unordered_map<std::string, Charalotte::FActorsInfoForPrint> FSceneDataManager::GetActorInfos()
{
	return ActorInfors;
}
std::unordered_map<std::string, Charalotte::FMeshInfoForPrint> FSceneDataManager::GetMeshInfors()
{
	return MeshsInfors;
}
void FSceneDataManager::LoadMap(const std::string& MapName) {
	
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
	FSceneDataManager::GetInstance().BuilMeshAsset(MapName);
	FSceneDataManager::GetInstance().BuildActors(MapName);

	FWinEventRegisterSystem::GetInstance().ExecuteMapLoadEvent(MapName);

	NowMapName = MapName;
}

Charalotte::FMeshInfoForPrint FSceneDataManager::GetMeshInfoByName(const std::string& MeshName)
{
	auto MeshInfoIter = MeshsInfors.find(MeshName);
	if (MeshInfoIter != MeshsInfors.end())
	{
		return MeshInfoIter->second;
	}
	return Charalotte::FMeshInfoForPrint();
}

std::unordered_map<std::string, std::vector<std::shared_ptr<Charalotte::FActorAsset>>>& FSceneDataManager::GetActorDictionary()
{
	return ActorDir;
}

void FSceneDataManager::CalcVerticesAndIndices(const std::string& GeometryName, const Charalotte::FTransform& Transform)
{
	Charalotte::FMeshInfoForPrint MeshInfo = FSceneDataManager::GetInstance().GetMeshInfoByName(GeometryName);
	std::shared_ptr<Charalotte::MeshGeometry> MeshGeo = std::make_shared<Charalotte::MeshGeometry>();

	std::string Name = GeometryName;
	if (MeshInfo.LodInfos.size() <= 0)
	{
		std::stringstream ss;
		ss << GeometryName << "No result";
		OutputDebugStringA(ss.str().c_str());
		return;
	}

	int VertexIndex = 0;
	// use normal to vertex color
	bool IsUseNormalToColor = false;
	if (MeshInfo.LodInfos[0].VerticesLocation.size() == MeshInfo.LodInfos[0].VerticesNormal.size())
	{
		IsUseNormalToColor = true;
	}

	for (const auto& VertexLocation : MeshInfo.LodInfos[0].VerticesLocation)
	{
		glm::vec4 VertexColor = glm::vec4(1.0f);

		Charalotte::Vertex vertex;
		glm::vec3 Float3 = glm::vec3(1.0f);
		// execute scale transport
		Float3.x = VertexLocation.x;
		Float3.y = VertexLocation.y;
		Float3.z = VertexLocation.z;

		vertex.Pos = Float3;
		if (IsUseNormalToColor)
		{
			VertexColor.x = MeshInfo.LodInfos[0].VerticesNormal[VertexIndex].x * 0.5f + 0.5f;
			VertexColor.y = MeshInfo.LodInfos[0].VerticesNormal[VertexIndex].y * 0.5f + 0.5f;
			VertexColor.z = MeshInfo.LodInfos[0].VerticesNormal[VertexIndex].z * 0.5f + 0.5f;
			VertexColor.w = MeshInfo.LodInfos[0].VerticesNormal[VertexIndex].w * 0.5f + 0.5f;
		}
		vertex.Color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		vertex.Normal = VertexColor;
		MeshGeo->vertices.push_back(vertex);
		VertexIndex++;
	}

	for (const auto& index : MeshInfo.LodInfos[0].Indices)
	{
		int32_t VertexIndex = index;
		MeshGeo->indices.push_back(static_cast<int16_t>(VertexIndex));
	}

	Charalotte::SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)(MeshInfo.LodInfos[0].Indices.size());
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	MeshGeo->DrawArgs[GeometryName] = submesh;
	MeshGeo->Name = GeometryName;

	FWinSceneAsset::AddMeshData(GeometryName, MeshGeo);
}

void FSceneDataManager::BuilMeshAsset(const std::string& MapName)
{
	const auto& ActorInfors = FSceneDataManager::GetInstance().GetActorInfos();
	auto ActorInfosIter = ActorInfors.find(MapName);
	if (ActorInfosIter != ActorInfors.end())
	{
		for (const auto& EnviroumentActor : ActorInfosIter->second.ActorsInfo)
		{
			std::string assetName = EnviroumentActor.AssetName;
			if (assetName.size() <= 0)
			{
				continue;
			}
			assetName.erase(assetName.size() - 1, 1);
			//OutputDebugStringA(assetName.c_str());
			CalcVerticesAndIndices(assetName, EnviroumentActor.Transform);
		}
	}
}

void FSceneDataManager::BuildActors(const std::string& MapName)
{
	ActorDir.insert({ MapName, {} });
	auto Iter = ActorDir.find(MapName);
	const auto& ActorInfors = FSceneDataManager::GetInstance().GetActorInfos();
	auto ActorInfosIter = ActorInfors.find(MapName);
	if (ActorInfosIter->second.ActorsInfo.size() <= 0) return;
	for (const auto& EnviroumentActor : ActorInfosIter->second.ActorsInfo)
	{
		std::string assetName = EnviroumentActor.AssetName;
		if (assetName.size() <= 0)
		{
			continue;
		}
		assetName.erase(assetName.size() - 1, 1);
		std::shared_ptr<Charalotte::FActorAsset> ActorAsset = std::make_shared<Charalotte::FActorAsset>();
		ActorAsset->MeshAsset = FWinSceneAsset::GetMeshAsset(assetName);
		ActorAsset->Transform = EnviroumentActor.Transform;

		Iter->second.push_back(ActorAsset);
	}
}

std::vector<std::shared_ptr<Charalotte::FActorAsset>>& FSceneDataManager::GetSceneActorByName(const std::string& MapName)
{
	const auto& ActorVecIter = ActorDir.find(MapName);
	if (ActorVecIter != ActorDir.end())
	{
		return ActorVecIter->second;
	}
	else
	{
		return EmptyActorVec;
	}
}

void FSceneDataManager::Update()
{
	if (FSceneDataManager::GetInstance().GetIsCanResizing())
	{
		FWinEventRegisterSystem::GetInstance().ExecuteOnResizeEvent(Charalotte::DXRenderResize);
		FSceneDataManager::GetInstance().SetIsCanResizing(false);
	}
	for (auto& ActorIns : FSceneDataManager::GetInstance().GetSceneActorByName(NowMapName))
	{
		// update the constant buffer with the latest worldviewproj glm::mat4
		Charalotte::ObjectConstants objConstants;
		glm::mat4 NowVPTrans;
		FSceneDataManager::GetInstance().GetCamera()->GetVPTransform(NowVPTrans);
		glm::mat4 NowWorldTrans = FMathHelper::GetWorldTransMatrix(ActorIns->Transform);
		glm::mat4 NowMVPTrans = NowVPTrans * NowWorldTrans;
		objConstants.TransMatrix = glm::transpose(NowMVPTrans);
		ActorIns->ObjectCB->CopyData(0, objConstants);
	}
}
#include "stdafx.h"
#include "FScene.h"
#include "FMeshAsset.h"

FScene::FScene() {
	MainCamera = std::make_shared<FCamera>();
	Cameras.push_back(MainCamera);
	CameraTrans = Charalotte::CameraTransform();
	DefaultCameraTrans = Charalotte::CameraTransform();

	ActorDir = {};
	EmptyActorVec = {};
	NowMapName = "";
}
FScene::~FScene() {
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

FCamera* FScene::GetCamera()
{
	return MainCamera.get();
}

Charalotte::CameraTransform& FScene::GetCameraTrans()
{
	return CameraTrans;
}

void FScene::InitCameraTrans()
{
	CameraTrans = DefaultCameraTrans;
}

std::unordered_map<std::string, Charalotte::FActorsInfoForPrint> FScene::GetActorInfos()
{
	return ActorInfors;
}

void FScene::LoadMap(const std::string& MapName) {
	
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
			FMeshAsset::GetInstance().AddMeshInfors(NameWithout, MeshInfo);
		}
	}
	FScene::GetInstance().BuilMeshAsset(MapName);
	FScene::GetInstance().BuildActors(MapName);

	FWinEventRegisterSystem::GetInstance().ExecuteMapLoadEvent(MapName);

	NowMapName = MapName;
}

std::unordered_map<std::string, std::vector<std::shared_ptr<Charalotte::FActorAsset>>>& FScene::GetActorDictionary()
{
	return ActorDir;
}

void FScene::CalcVerticesAndIndices(const std::string& GeometryName, const Charalotte::FTransform& Transform)
{
	Charalotte::FMeshInfoForPrint MeshInfo = FMeshAsset::GetInstance().GetMeshInfoByName(GeometryName);
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

	FDXRenderMeshDataBuffer::AddMeshData(GeometryName, MeshGeo);
}

void FScene::BuilMeshAsset(const std::string& MapName)
{
	const auto& ActorInfors = FScene::GetInstance().GetActorInfos();
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

void FScene::BuildActors(const std::string& MapName)
{
	ActorDir.clear();
	ActorDir.insert({ MapName, {} });
	auto Iter = ActorDir.find(MapName);
	const auto& ActorInfors = FScene::GetInstance().GetActorInfos();
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

		ActorAsset->MeshAsset = FDXRenderMeshDataBuffer::GetMeshAsset(assetName);
		if (ActorAsset->MeshAsset != nullptr)
		{
			ActorAsset->Transform = EnviroumentActor.Transform;

			Iter->second.push_back(ActorAsset);
		}
	}
}

std::vector<std::shared_ptr<Charalotte::FActorAsset>>& FScene::GetSceneActorByName(const std::string& MapName)
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

void FScene::Update()
{
	FWinEventRegisterSystem::GetInstance().ExecuteOnResizeEvent(Charalotte::DXRenderResize);

	for (auto& ActorIns : FScene::GetInstance().GetSceneActorByName(NowMapName))
	{
		// update the constant buffer with the latest worldviewproj glm::mat4
		Charalotte::ObjectConstants objConstants;
		glm::mat4 NowVPTrans;
		FScene::GetInstance().GetCamera()->GetVPTransform(NowVPTrans);
		glm::mat4 NowWorldTrans = FMathHelper::GetWorldTransMatrix(ActorIns->Transform);
		auto& RotateStruct = ActorIns->Transform.Rotation;
		glm::vec4 Rotate(RotateStruct.X, RotateStruct.Y, RotateStruct.Z, RotateStruct.W);
		glm::mat4 NowRotate = FMathHelper::GetRotateMatrix(Rotate);
		glm::mat4 NowMVPTrans = NowVPTrans * NowWorldTrans;
		objConstants.TransMatrix = glm::transpose(NowMVPTrans);
		objConstants.Rotate = (NowRotate);
		ActorIns->ObjectCB->CopyData(0, objConstants);
	}
}
#include "stdafx.h"
#include "FScene.h"
#include "FMeshAsset.h"
#include "CharlotteEngine.h"

FScene::FScene() {
	MainCamera = std::make_shared<FCamera>();
	Cameras.push_back(MainCamera);
	CameraTrans = Charalotte::CameraTransform();
	DefaultCameraTrans = Charalotte::CameraTransform();

	ActorDir = {};
	EmptyActorVec = {};
	NowMapName = "";
	MeshPrimitiveTimes = {};
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

std::unordered_map<std::string, Charalotte::FActorPrimitive>& FScene::GetActorInfos()
{
	return ActorInfors;
}

void FScene::LoadMap(const std::string& MapName) {
	
	Charalotte::FActorPrimitive TempActorInfors;
	FDataProcessor::LoadActors(MapName, TempActorInfors);
	const auto& TextureNames = Charalotte::CharalotteEngine::GetInstance().GetTextureArray();
	int TextureNum = TextureNames.size();
	int TempTextureIndex = 0;
	std::set<std::string> AssetNames;
	bool cool = false;
	for (auto& ActorInfor : TempActorInfors.ActorsInfo)
	{
		std::string AssetName = ActorInfor.MeshPrimitiveName;
		if (AssetName.size() > 0)
		{
			AssetName.erase(AssetName.size() - 1, 1);
		}

		const auto& MeshPrimitiveIter = MeshPrimitiveTimes.find(AssetName);
		if (MeshPrimitiveIter != MeshPrimitiveTimes.end())
		{
			MeshPrimitiveIter->second++;
			ActorInfor.ActorPrimitiveName = AssetName + "_" + std::to_string(MeshPrimitiveIter->second);
		}
		else
		{
			MeshPrimitiveTimes.insert({ AssetName, 0 });
			ActorInfor.ActorPrimitiveName = AssetName;
		}

		AssetName += ".dat";
		AssetNames.insert(AssetName);

		// test use different texture
		if (cool)
		{
			ActorInfor.Material->SetTexture("stone");
			cool = false;
		}
		else
		{
			ActorInfor.Material->SetTexture("bricks");
			cool = true;
		}
		ActorInfor.MaterialPath = "Content/Textures/StoneTexture.dds";
		
		/*if (TempTextureIndex < TextureNum)
		{
			ActorInfor.Material->SetTexture(TextureNames[1]);
			TempTextureIndex++;
		}
		else

		{
			TempTextureIndex = 0;
		}*/
	}
	ActorInfors.insert({ MapName, TempActorInfors });

	for (const auto& AssetName : AssetNames)
	{
		Charalotte::FMeshPrimitive MeshInfo;
		FDataProcessor::LoadMesh(AssetName, MeshInfo);
		std::string NameWithout = AssetName;

		if (NameWithout.size() > 4)
		{
			NameWithout.erase(NameWithout.size() - 4, 4);
			FMeshAsset::GetInstance().AddMeshInfors(NameWithout, MeshInfo);
		}
	}

	FWinEventRegisterSystem::GetInstance().ExecuteMapLoadEvent(MapName);

	NowMapName = MapName;
}


std::unordered_map<std::string, std::vector<std::shared_ptr<Charalotte::FDXActorPrimitive>>>& FScene::GetActorDictionary()
{
	return ActorDir;
}


std::vector<std::shared_ptr<Charalotte::FDXActorPrimitive>>& FScene::GetSceneActorByName(const std::string& MapName)
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
		//// update the constant buffer with the latest worldviewproj glm::mat4
		/*Charalotte::ObjectConstants objConstants;
		glm::mat4 NowVPTrans;
		FScene::GetInstance().GetCamera()->GetVPTransform(NowVPTrans);
		glm::mat4 NowWorldTrans = FMathHelper::GetWorldTransMatrix(ActorIns->Transform);
		auto& RotateStruct = ActorIns->Transform.Rotation;
		glm::vec4 Rotate(RotateStruct.X, RotateStruct.Y, RotateStruct.Z, RotateStruct.W);
		glm::mat4 NowRotate = FMathHelper::GetRotateMatrix(Rotate);
		glm::mat4 NowMVPTrans = NowVPTrans * NowWorldTrans;
		objConstants.TransMatrix = glm::transpose(NowMVPTrans);
		objConstants.Rotate = (NowRotate);
		ActorIns->ObjectCB->CopyData(0, objConstants);*/
	}
}
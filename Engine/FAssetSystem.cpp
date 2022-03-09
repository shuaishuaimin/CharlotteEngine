#include "stdafx.h"
#include "FAssetSystem.h"
#include "FWinEventRegisterSystem.h"

void FAssetSystem::LoadMap(const std::string& MapName) {
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

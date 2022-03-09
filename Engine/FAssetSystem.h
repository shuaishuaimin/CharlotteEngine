#pragma once
#include <string>
#include <set>
#include "BaseStructAllPlatform.h"
#include "FDataProcessor.h"

class FAssetSystem
{
public:
	FAssetSystem(){}
	~FAssetSystem(){}

void LoadMap(const std::string& MapName);

std::unordered_map<std::string, Charalotte::FActorsInfoForPrint> GetActorInfos()
{
	return ActorInfors;
}

std::unordered_map<std::string, Charalotte::FMeshInfoForPrint> GetMeshInfors()
{
	return MeshsInfors;
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
	std::unordered_map<std::string, Charalotte::FActorsInfoForPrint> ActorInfors;
	std::unordered_map<std::string, Charalotte::FMeshInfoForPrint> MeshsInfors;
};
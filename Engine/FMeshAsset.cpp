#include "stdafx.h"
#include "FMeshAsset.h"

FMeshAsset::FMeshAsset()
{
	MeshsInfors = {};
}
FMeshAsset::~FMeshAsset()
{
	MeshsInfors.clear();
}
Charalotte::FMeshInfoForPrint FMeshAsset::GetMeshInfoByName(const std::string& MeshName)
{
	auto MeshInfoIter = MeshsInfors.find(MeshName);
	if (MeshInfoIter != MeshsInfors.end())
	{
		return MeshInfoIter->second;
	}
	return Charalotte::FMeshInfoForPrint();
}
std::unordered_map<std::string, Charalotte::FMeshInfoForPrint> FMeshAsset::GetMeshInfors()
{
	return MeshsInfors;
}
void FMeshAsset::AddMeshInfors(const std::string& MeshName, Charalotte::FMeshInfoForPrint MeshInfo)
{
	if (MeshNames.find(MeshName) != MeshNames.end())
	{
		return;
	}
	MeshNames.insert(MeshName);
	MeshsInfors.insert({ MeshName , MeshInfo });
}






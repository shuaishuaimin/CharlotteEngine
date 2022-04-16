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
Charalotte::FMeshPrimitive FMeshAsset::GetMeshInfoByName(const std::string& MeshName)
{
	auto MeshInfoIter = MeshsInfors.find(MeshName);
	if (MeshInfoIter != MeshsInfors.end())
	{
		return MeshInfoIter->second;
	}
	return Charalotte::FMeshPrimitive();
}
std::unordered_map<std::string, Charalotte::FMeshPrimitive>& FMeshAsset::GetMeshInfors()
{
	return MeshsInfors;
}
void FMeshAsset::AddMeshInfors(const std::string& MeshName, Charalotte::FMeshPrimitive MeshInfo)
{
	if (MeshNames.find(MeshName) != MeshNames.end())
	{
		return;
	}
	MeshNames.insert(MeshName);
	MeshsInfors.insert({ MeshName , MeshInfo });
}






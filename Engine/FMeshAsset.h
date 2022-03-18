#pragma once
#include <unordered_map>
#include <string>
#include <set>
#include "Singleton.h"
#include "BaseStructAllPlatform.h"

class FMeshAsset : public Singleton<FMeshAsset>
{
public:
	FMeshAsset() ;
	~FMeshAsset() ;
	Charalotte::FMeshPrimitive GetMeshInfoByName(const std::string& MeshName);
	std::unordered_map<std::string, Charalotte::FMeshPrimitive> GetMeshInfors();
	void AddMeshInfors(const std::string& MeshName, Charalotte::FMeshPrimitive MeshInfo);

private:
	// save the mesh information without render, the real data is in win scene asset
	std::unordered_map<std::string, Charalotte::FMeshPrimitive> MeshsInfors;
	std::set<std::string> MeshNames;
};

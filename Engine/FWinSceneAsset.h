#pragma once
#include <iostream>
#include "WinBaseData.h"

class FWinSceneAsset
{
public:
	FWinSceneAsset(){}
	~FWinSceneAsset(){
		MeshAssets = std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>>();
	}
	static std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>> MeshAssets;
	static void AddMeshData(const std::string& AssetName, std::shared_ptr<Charalotte::MeshGeometry> MeshAsset)
	{
		if (MeshAssets.find(AssetName) != MeshAssets.end())
		{
			return;
		}
		MeshAssets.insert(std::make_pair(AssetName, MeshAsset));
	}

	static std::shared_ptr<Charalotte::MeshGeometry> GetMeshAsset(const std::string& Assetname)
	{
		auto AssetIter = MeshAssets.find(Assetname);
		if (AssetIter != MeshAssets.end())
		{
			return AssetIter->second;
		}
		else
		{
			return nullptr;
		}
	}

	static std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>> GetMeshAssets()
	{
		return MeshAssets;
	}

	
};

std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>> FWinSceneAsset::MeshAssets = std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>>();
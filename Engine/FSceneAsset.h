#pragma once
#include <iostream>
#include "BaseData.h"
#include "FUtil.h"

class FSceneAsset
{
public:
	FSceneAsset(){}
	~FSceneAsset(){
		MeshAssets = std::unordered_map<std::string, std::shared_ptr<MeshGeometry>>();
	}
	static std::unordered_map<std::string, std::shared_ptr<MeshGeometry>> MeshAssets;
	static void AddMeshData(const std::string& AssetName, std::shared_ptr<MeshGeometry> MeshAsset)
	{
		if (MeshAssets.find(AssetName) != MeshAssets.end())
		{
			return;
		}
		MeshAssets.insert(std::make_pair(AssetName, MeshAsset));
	}

	static std::shared_ptr<MeshGeometry> GetMeshAsset(const std::string& Assetname)
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

	static std::unordered_map<std::string, std::shared_ptr<MeshGeometry>> GetMeshAssets()
	{
		return MeshAssets;
	}

	
};

std::unordered_map<std::string, std::shared_ptr<MeshGeometry>> FSceneAsset::MeshAssets = std::unordered_map<std::string, std::shared_ptr<MeshGeometry>>();
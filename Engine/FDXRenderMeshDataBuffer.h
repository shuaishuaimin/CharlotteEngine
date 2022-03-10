#pragma once
#include <iostream>
#include "WinBaseData.h"

class FDXRenderMeshDataBuffer
{
public:
	FDXRenderMeshDataBuffer();
	~FDXRenderMeshDataBuffer();
	
	static void AddMeshData(const std::string& AssetName, std::shared_ptr<Charalotte::MeshGeometry> MeshAsset);

	static std::shared_ptr<Charalotte::MeshGeometry> GetMeshAsset(const std::string& Assetname);

	static std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>> GetMeshAssets();

private:
	static std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>> MeshAssets;
};

#pragma once
#include <iostream>
#include "WinFrameResource.h"
#include "Singleton.h"

class FDXResources : public Singleton<FDXResources>
{
public:
	FDXResources();
	~FDXResources();
	
	void AddMeshData(const std::string& AssetName, std::shared_ptr<Charalotte::MeshGeometry>& MeshAsset);

	Charalotte::MeshGeometry* GetMeshAsset(const std::string& Assetname);

	std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>>& GetMeshAssets();

	Charalotte::DXTextureResource* GetTextResourceByName(const std::string& TextureName);

	void AddDxTextureResource(const std::string& Name, std::shared_ptr<Charalotte::DXTextureResource>& DXTexture);
private:
	std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>> MeshResources;
	std::unordered_map<std::string, std::weak_ptr<Charalotte::DXTextureResource>> TextureResources;
};

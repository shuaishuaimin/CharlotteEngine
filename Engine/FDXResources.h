#pragma once
#include <iostream>
#include "DXPrimitives.h"
#include "Singleton.h"

class FDXResources : public Singleton<FDXResources>
{
public:
	FDXResources();
	~FDXResources();
	
	void AddMeshData(const std::string& AssetName, std::shared_ptr<Charalotte::DXMeshPrimitive>& MeshAsset);

	Charalotte::DXMeshPrimitive* GetMeshAsset(const std::string& Assetname);

	std::unordered_map<std::string, std::shared_ptr<Charalotte::DXMeshPrimitive>>& GetMeshAssets();

	Charalotte::DXTextureResource* GetTextResourceByName(const std::string& TextureName);

	void AddDxTextureResource(const std::string& Name, std::shared_ptr<Charalotte::DXTextureResource>& DXTexture);
private:
	std::unordered_map<std::string, std::shared_ptr<Charalotte::DXMeshPrimitive>> MeshResources;
	std::unordered_map<std::string, std::shared_ptr<Charalotte::DXTextureResource>> TextureResources;
	std::unordered_map<std::string, std::shared_ptr<Charalotte::FDXActorPrimitive>> ActorResources;
};

#pragma once
#include <iostream>
#include "DXPrimitives.h"
#include "FRenderScene.h"

class FWinRenderScene : public FRenderScene
{
public:
	FWinRenderScene();
	virtual ~FWinRenderScene();
	
	void AddDXMeshPrimitive(const std::string& AssetName, std::shared_ptr<Charalotte::FDXMeshPrimitive>& MeshAsset);

	Charalotte::FDXMeshPrimitive* GetDXMeshResourceByName(const std::string& Assetname);

	std::unordered_map<std::string, std::shared_ptr<Charalotte::FDXMeshPrimitive>>& GetDXMeshResources();

	Charalotte::DXTextureResource* GetDXTextResourceByName(const std::string& TextureName);

	void AddDxTextureResource(const std::string& Name, std::shared_ptr<Charalotte::DXTextureResource>& DXTexture);

	void AddDXActorPrimitive(const std::string& ActorName, std::shared_ptr<Charalotte::FDXActorPrimitive>& DXActor);

	void ClearDXActorPrimitives();

	std::unordered_map<std::string, std::shared_ptr<Charalotte::FDXActorPrimitive>>& GetDXActorResources();

	Charalotte::FDXActorPrimitive* GetDXActorResourcesByName(const std::string& DXActorName);
private:
	std::unordered_map<std::string, std::shared_ptr<Charalotte::FDXMeshPrimitive>> MeshResources;
	std::unordered_map<std::string, std::shared_ptr<Charalotte::DXTextureResource>> TextureResources;
	std::unordered_map<std::string, std::shared_ptr<Charalotte::FDXActorPrimitive>> ActorResources;

};

#include "stdafx.h"
#include "FWinRenderScene.h"


FWinRenderScene::FWinRenderScene() {
	MeshResources = {};
	TextureResources = {};
	}
FWinRenderScene::~FWinRenderScene() {
	MeshResources.clear();
}

void FWinRenderScene::AddDXMeshPrimitive(const std::string& AssetName, std::shared_ptr<Charalotte::FDXMeshPrimitive>& MeshAsset)
{
	if (MeshResources.find(AssetName) != MeshResources.end())
	{
		return;
	}
	MeshResources.insert(std::make_pair(AssetName, std::move(MeshAsset)));
}

Charalotte::FDXMeshPrimitive* FWinRenderScene::GetDXMeshResourceByName(const std::string& Assetname)
{
	auto AssetIter = MeshResources.find(Assetname);
	if (AssetIter != MeshResources.end())
	{
		return AssetIter->second.get();
	}
	else
	{
		return nullptr;
	}
}

std::unordered_map<std::string, std::shared_ptr<Charalotte::FDXMeshPrimitive>>& FWinRenderScene::GetDXMeshResources()
{
	return MeshResources;
}

Charalotte::DXTextureResource* FWinRenderScene::GetDXTextResourceByName(const std::string& TextureName)
{
	const auto& Iter = TextureResources.find(TextureName);
	if (Iter != TextureResources.end())
	{
		return Iter->second.get();
	}
	else
	{
		return nullptr;
	}
}

void FWinRenderScene::AddDxTextureResource(const std::string& Name, std::shared_ptr<Charalotte::DXTextureResource>& DXTexture)
{
	TextureResources.insert({Name, std::move(DXTexture)});
}

void FWinRenderScene::AddDXActorPrimitive(const std::string& ActorName, std::shared_ptr<Charalotte::FDXActorPrimitive>& DXActor)
{
	ActorResources.insert({ActorName, std::move(DXActor)});
}

void FWinRenderScene::ClearDXActorPrimitives()
{
	ActorResources.clear();
}

std::unordered_map<std::string, std::shared_ptr<Charalotte::FDXActorPrimitive>>& FWinRenderScene::GetDXActorResources()
{
	return ActorResources;
}

Charalotte::FDXActorPrimitive* FWinRenderScene::GetDXActorResourcesByName(const std::string& DXActorName)
{
	const auto& Iter = ActorResources.find(DXActorName);
	if (Iter != ActorResources.end())
	{
		return Iter->second.get();
	}
	else
	{
		return nullptr;
	}
}


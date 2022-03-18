#include "stdafx.h"
#include "FDXResources.h"


FDXResources::FDXResources() {
	MeshResources = {};
	TextureResources = {};
	}
FDXResources::~FDXResources() {
	MeshResources.clear();
}

void FDXResources::AddDXMeshPrimitive(const std::string& AssetName, std::shared_ptr<Charalotte::FDXMeshPrimitive>& MeshAsset)
{
	if (MeshResources.find(AssetName) != MeshResources.end())
	{
		return;
	}
	MeshResources.insert(std::make_pair(AssetName, std::move(MeshAsset)));
}

Charalotte::FDXMeshPrimitive* FDXResources::GetDXMeshResourceByName(const std::string& Assetname)
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

std::unordered_map<std::string, std::shared_ptr<Charalotte::FDXMeshPrimitive>>& FDXResources::GetDXMeshResources()
{
	return MeshResources;
}

Charalotte::DXTextureResource* FDXResources::GetDXTextResourceByName(const std::string& TextureName)
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

void FDXResources::AddDxTextureResource(const std::string& Name, std::shared_ptr<Charalotte::DXTextureResource>& DXTexture)
{
	TextureResources.insert({Name, std::move(DXTexture)});
}

void FDXResources::AddDXActorPrimitive(const std::string& ActorName, std::shared_ptr<Charalotte::FDXActorPrimitive>& DXActor)
{
	ActorResources.insert({ActorName, std::move(DXActor)});
}

void FDXResources::ClearDXActorPrimitives()
{
	ActorResources.clear();
}
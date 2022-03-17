#include "stdafx.h"
#include "FDXResources.h"


FDXResources::FDXResources() {
	MeshResources = {};
	TextureResources = {};
	}
FDXResources::~FDXResources() {
	MeshResources.clear();
}

void FDXResources::AddMeshData(const std::string& AssetName, std::shared_ptr<Charalotte::DXMeshPrimitive>& MeshAsset)
{
	if (MeshResources.find(AssetName) != MeshResources.end())
	{
		return;
	}
	MeshResources.insert(std::make_pair(AssetName, std::move(MeshAsset)));
}

Charalotte::DXMeshPrimitive* FDXResources::GetMeshAsset(const std::string& Assetname)
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

std::unordered_map<std::string, std::shared_ptr<Charalotte::DXMeshPrimitive>>& FDXResources::GetMeshAssets()
{
	return MeshResources;
}

Charalotte::DXTextureResource* FDXResources::GetTextResourceByName(const std::string& TextureName)
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


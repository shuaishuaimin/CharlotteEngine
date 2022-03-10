#include "stdafx.h"
#include "FDXRenderMeshDataBuffer.h"


// save the mesh asset 
std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>> FDXRenderMeshDataBuffer::MeshAssets = 
				std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>>();

FDXRenderMeshDataBuffer::FDXRenderMeshDataBuffer() {}
FDXRenderMeshDataBuffer::~FDXRenderMeshDataBuffer() {
	MeshAssets.clear();
}

void FDXRenderMeshDataBuffer::AddMeshData(const std::string& AssetName, std::shared_ptr<Charalotte::MeshGeometry> MeshAsset)
{
	if (MeshAssets.find(AssetName) != MeshAssets.end())
	{
		return;
	}
	MeshAssets.insert(std::make_pair(AssetName, MeshAsset));
}

std::shared_ptr<Charalotte::MeshGeometry> FDXRenderMeshDataBuffer::GetMeshAsset(const std::string& Assetname)
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

std::unordered_map<std::string, std::shared_ptr<Charalotte::MeshGeometry>> FDXRenderMeshDataBuffer::GetMeshAssets()
{
	return MeshAssets;
}


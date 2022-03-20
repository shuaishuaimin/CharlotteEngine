#pragma once
#include <vector>
#include "BaseStructAllPlatform.h"
#include "FMaterial.h"

namespace Charalotte
{
	struct FLODInfo
	{

		int VerticesNum;

		int TrianglesNum;

		int IndicesNum;

		std::vector<Charalotte::FVector> VerticesLocation;

		std::vector<int> Indices;

		std::vector<Charalotte::FVector4> VerticesNormal;

		std::vector<Charalotte::FVector2D> UVs;
	};

	struct FMeshPrimitive
	{
		std::string MeshName;

		int LODNums = 0;

		std::vector<Charalotte::FLODInfo> LodInfos;
	};

	struct FActorInfo
	{
		FActorInfo()
		{
			Material = std::make_shared<FMaterial>();
		}
		Charalotte::FTransform Transform;

		std::string MeshPrimitiveName;

		std::string ActorPrimitiveName;

		std::shared_ptr<FMaterial> Material ; 
	};

	struct FActorPrimitive
	{
		std::vector<Charalotte::FActorInfo> ActorsInfo;
	};
}
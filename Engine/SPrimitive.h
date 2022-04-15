#pragma once
#include <vector>
#include "STransformDef.h"
#include "FMaterial.h"

class SPrimitive
{

};

namespace Charalotte
{
	class FMaterial;
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

		std::string MaterialPath;
	};

	struct FActorPrimitive
	{
		std::vector<Charalotte::FActorInfo> ActorsInfo;
	};
}
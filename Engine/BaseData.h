#pragma once
#include "stdafx.h"
#include "BaseStructAllPlatform.h"
#include "FMathHelper.h"

namespace Charalotte
{
	struct Vertex
	{
		glm::vec3 Pos;
		glm::vec4 Color;
		glm::vec4 Normal;
	};

	struct ObjectConstants
	{
		glm::mat4 TransMatrix = FMathHelper::Identity4x4();
	};

	struct FLODInfo
	{

		int VerticesNum;

		int TrianglesNum;

		int IndicesNum;

		std::vector<Charalotte::FVector> VerticesLocation;

		std::vector<int> Indices;

		std::vector<Charalotte::FVector4> VerticesNormal;
	};

	struct FMeshInfoForPrint
	{
		std::string MeshName;

		int LODNums;

		std::vector<Charalotte::FLODInfo> LodInfos;
	};

	struct FActorInfo
	{
		Charalotte::FTransform Transform;

		std::string AssetName;

	};

	struct FActorsInfoForPrint
	{
		std::vector<Charalotte::FActorInfo> ActorsInfo;
	};

	struct CameraData
	{
		glm::vec4 Location = glm::vec4();
		glm::vec4 Target = glm::vec4();
		glm::vec4 Up = glm::vec4();
		float AspectRatio = 0.0f;
		float FovAngleY = 0.25f * FMathHelper::Pi;
		float Near = 1.0f;
		float Far = 1000.0f;
		float Width = 0.0f;
		float Height = 0.0f;
	};

	struct VPTransformData
	{
		glm::mat4 ViewTransform;
		glm::mat4 ProjectionTransform;

		glm::mat4 VPMatrix;
	};

	struct CameraTransform
	{
		FVector Translation;
		float row = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
	};
}


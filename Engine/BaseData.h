#pragma once
#include "stdafx.h"
#include "BaseStructAllPlatform.h"
#include "FMathHelper.h"

namespace Charalotte
{
	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT4 Normal;
	};

	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 TransMatrix = FMathHelper::Identity4x4();
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
		DirectX::XMVECTOR Location = DirectX::XMVectorZero();
		DirectX::XMVECTOR Target = DirectX::XMVectorZero();
		DirectX::XMVECTOR Up = DirectX::XMVectorZero();
		float AspectRatio = 0.0f;
		float FovAngleY = 0.25f * FMathHelper::Pi;
		float Near = 1.0f;
		float Far = 1000.0f;
	};

	struct VPTransformData
	{
		DirectX::XMMATRIX ViewTransform;
		DirectX::XMMATRIX ProjectionTransform;

		DirectX::XMMATRIX VPMatrix;
	};

	struct CameraTransform
	{
		FVector Translation;
		float row = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
	};
}


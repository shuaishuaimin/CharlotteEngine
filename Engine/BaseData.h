#pragma once
#include "stdafx.h"
#include "FMathHelper.h"

namespace Charalotte
{
	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Color;
	};

	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 TransMatrix = FMathHelper::Identity4x4();
	};

	struct FVector
	{
	public:

		/** Vector's X component. */
		float x = 0.0f;

		/** Vector's Y component. */
		float y = 0.0f;

		/** Vector's Z component. */
		float z = 0.0f;

		friend std::ostream& operator<<(std::ostream& os, const Charalotte::FVector& Vector)
		{
			os << " x: " << Vector.x << " y: " << Vector.y << " z: " << Vector.z;
			return os;
		}
	};
	struct FVector4
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;

		friend std::ostream& operator<<(std::ostream& os, const Charalotte::FVector4& Vector4)
		{
			os << " x: " << Vector4.x << " y: " << Vector4.y << " z: " << Vector4.z << " w: " << Vector4.w;
			return os;
		}
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



	struct FTransform
	{
		Charalotte::FVector4 Rotation;
		Charalotte::FVector Translation;
		Charalotte::FVector Scale3D;
		friend std::ostream& operator << (std::ostream& os, const Charalotte::FTransform& TransForm)
		{
			os << "Rotation: " << TransForm.Rotation << "\n" << "Translation: " << TransForm.Translation << "\n" << "Scale3d: " << TransForm.Scale3D << "\n";
			return os;
		}
	};

	struct FActorInfo
	{
		Charalotte::FTransform Transform;

		std::string AssetName;

		friend std::ostream& operator << (std::ostream& os, const FActorInfo& FActorInfo)
		{
			os << "Translation: " << "\n" << FActorInfo.Transform << "\n";
			{
				os << "Asset name:" << FActorInfo.AssetName << "\n";
			}
			return os;
		}
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


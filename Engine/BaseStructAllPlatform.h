#pragma once
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/quaternion.hpp"

namespace Charalotte
{
	struct FVector
	{
	public:

		/** Vector's X component. */
		float x = 0.0f;

		/** Vector's Y component. */
		float y = 0.0f;

		/** Vector's Z component. */
		float z = 0.0f;

	};
	struct FVector4
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;
	};

	struct FQuaternion
	{
		float Y = 0.0f;
		float Z = 0.0f;
		float X = 0.0f;
		float W = 1.0f;
	};

	struct FTransform
	{
		Charalotte::FQuaternion Rotation;
		Charalotte::FVector Translation;
		Charalotte::FVector Scale3D;
	};

	struct FPoint2D
	{
		float x;
		float y;
	};

	struct Vertex
	{
		glm::vec3 Pos;
		glm::vec4 Color;
		glm::vec4 Normal;
	};

	struct ObjectConstants
	{
		glm::mat4 TransMatrix = glm::mat4(1.0f);
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
		float FovAngleY = 0.25f * glm::pi<float>() ;
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

	enum MapLoadType
	{
		BaseMapLoad
	};

	enum ResizeType
	{
		DXRenderResize
	};
}

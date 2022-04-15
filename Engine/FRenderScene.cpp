#include "stdafx.h"
#include "FRenderScene.h"
#include "FScene.h"
#include "FMeshAsset.h"

namespace Charalotte
{
	void FRenderScene::BuildResource(const std::string& MapName)
	{
		const auto& ActorInfos = FScene::GetInstance().GetActorInfos();
		const auto& ActorPrimitiveIter = ActorInfos.find(MapName);
		if (ActorPrimitiveIter != ActorInfos.end())
		{
			for (const auto& Actor : ActorPrimitiveIter->second.ActorsInfo)
			{
				std::string BufferName = Actor.MeshPrimitiveName;
				if (BufferName.size() <= 0)
				{
					continue;
				}
				BufferName.erase(BufferName.size() - 1, 1);
				CreateBufferResources(BufferName);
				
			}
			CreateRenderMeshs(ActorPrimitiveIter->second);
		}
	}

	void FRenderScene::CreateRenderMeshs(const Charalotte::FActorPrimitive& ActorPrimitive)
	{
		const auto& ActorInfors = ActorPrimitive.ActorsInfo;
		for (const auto& Actors : ActorPrimitive.ActorsInfo)
		{
			std::string ActorMeshPrimitiveName = Actors.MeshPrimitiveName;
			if (ActorMeshPrimitiveName.size() <= 0)
			{
				continue;
			}
			ActorMeshPrimitiveName.erase(ActorMeshPrimitiveName.size() - 1, 1);
			const auto& BufferResourceIter = BufferResources.find(ActorMeshPrimitiveName);
			if (BufferResourceIter != BufferResources.end())
			{
				auto Rs = BufferResourceIter->second;
				std::shared_ptr<Charalotte::FRenderMesh> RenderMesh = std::make_shared<Charalotte::FRenderMesh>();
#ifdef RENDER_PLATFORM_DX12
				Charalotte::FDXBoundingBox submesh;
				submesh.IndexCount = Rs->GetIndicesLength() ;
				submesh.StartIndexLocation = 0;
				submesh.BaseVertexLocation = 0;
				RenderMesh->SetBoundingBox(submesh);
#endif
				RenderMesh->SetBufferResource(Rs.get());
				const auto& MaterialIter = Materials.find(Actors.MaterialPath);
				if (MaterialIter != Materials.end())
				{
					RenderMesh->SetMaterial(MaterialIter->second.get());
				}
				else
				{
					RenderMesh->SetMaterial(nullptr);
				}
				RenderMesh->SetTransformData(Actors.Transform);
			}
			else
			{
				continue;
			}
		}
	}

	void FRenderScene::ClearRenderScene()
	{
		for (auto& BufferResource : BufferResources)
		{
			BufferResource.second->ClearBuffers();
			BufferResource.second = nullptr;
		}
		BufferResources.clear();
		for (auto& RenderMesh : RenderMeshs)
		{
			RenderMesh.second = nullptr;
		}
		RenderMeshs.clear();
		for (auto& mat : Materials)
		{
			mat.second->ClearMaterial();
			mat.second = nullptr;
		}
		Materials.clear();
	}

	void FRenderScene::CreateBufferResources(const std::string& BufferName)
	{
		// if there is buffer exist, return;
		if (BufferResources.find(BufferName) != BufferResources.end())
		{
			return;
		}
		const Charalotte::FMeshPrimitive& MeshPri = FMeshAsset::GetInstance().GetMeshInfors().find(BufferName)->second;
		std::string Name = BufferName;
		if (MeshPri.LodInfos.size() <= 0)
		{
			std::stringstream ss;
			ss << BufferName << "No result";
			OutputDebugStringA(ss.str().c_str());
			return;
		}
		
		std::shared_ptr<Charalotte::FVerticesAndIndicesBuffer> VBIBBuffer = std::make_shared<Charalotte::FVerticesAndIndicesBuffer>();
		int VertexIndex = 0;
		// use normal to vertex color
		bool IsUseNormalToColor = false;
		if (MeshPri.LodInfos[0].VerticesLocation.size() == MeshPri.LodInfos[0].VerticesNormal.size())
		{
			IsUseNormalToColor = true;
		}

		for (const auto& VertexLocation : MeshPri.LodInfos[0].VerticesLocation)
		{
			glm::vec4 VertexColor = glm::vec4(1.0f);

			Charalotte::Vertex vertex;
			glm::vec3 Float3 = glm::vec3(1.0f);
			// execute scale transport
			Float3.x = VertexLocation.x;
			Float3.y = VertexLocation.y;
			Float3.z = VertexLocation.z;

			const auto& MeshLod = MeshPri.LodInfos[0];
			vertex.Pos = Float3;
			if (IsUseNormalToColor)
			{
				VertexColor.x = MeshLod.VerticesNormal[VertexIndex].x * 0.5f + 0.5f;
				VertexColor.y = MeshLod.VerticesNormal[VertexIndex].y * 0.5f + 0.5f;
				VertexColor.z = MeshLod.VerticesNormal[VertexIndex].z * 0.5f + 0.5f;
				VertexColor.w = MeshLod.VerticesNormal[VertexIndex].w * 0.5f + 0.5f;
			}
			vertex.Color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			vertex.Normal = VertexColor;
			vertex.UV.x = MeshLod.UVs[VertexIndex].X;
			vertex.UV.y = MeshLod.UVs[VertexIndex].Y;
			VBIBBuffer->InsertVertex(vertex);
			VertexIndex++;
		}

		for (const auto& index : MeshPri.LodInfos[0].Indices)
		{
			int32_t VertexIndex = index;
			VBIBBuffer->InsertIndices(static_cast<int16_t>(VertexIndex));
		}

		Charalotte::FDXBoundingBox submesh;
		submesh.IndexCount = (UINT)(MeshPri.LodInfos[0].Indices.size());
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;
		VBIBBuffer->SetVBIBName(BufferName);
		BufferResources.insert({BufferName, VBIBBuffer});
	}
}


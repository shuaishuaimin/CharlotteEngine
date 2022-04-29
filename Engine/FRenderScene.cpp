#include "stdafx.h"
#include "FRenderScene.h"
#include "FScene.h"
#include "FMeshAsset.h"
#include "FRHIManager.h"
#include "CharlotteEngine.h"

//#include "FCoreShader.h"
#ifdef RENDER_PLATFORM_DX12
#include "FWinEventRegisterSystem.h"
#include "FDXRenderMesh.h"
#endif
#include "FTexture.h"
#include "FCoreShader.h"

namespace Charalotte
{
	FRenderScene::FRenderScene()
	{
#ifdef RENDER_PLATFORM_DX12
		FWinEventRegisterSystem::GetInstance().RegisterMapLoadEventForDender(MapLoadType::RenderSceneLoad, [this](std::string Mapname){
			this->BuildResource(Mapname);
		});
#endif
		std::string PathFront = "Content/Textures/";
		FileNames = { PathFront + "bricks" + ".dds", PathFront + "ice" + ".dds" , PathFront + "tile" + ".dds" , PathFront + "stone" + ".dds" 
						, PathFront + "StoneTexture" + ".dds", PathFront + "StoneNormal" + ".dds"};
		RHIPtr = nullptr;
	}

	void FRenderScene::AddNewTexture(std::string TexturePath)
	{
		std::shared_ptr<FTexture> Tex = std::make_shared<FTexture>(TexturePath);
		FRHIManager::GetInstance().GetRHIPtr()->CreateTextureResource(Tex.get());
		Textures.insert({TexturePath, std::move(Tex)});
	}

	void FRenderScene::EraseTexture(std::string TexturePath)
	{
		const auto& TexIter = Textures.find(TexturePath);
		if (TexIter != Textures.end())
		{
			TexIter->second = nullptr;
			Textures.erase(TexturePath);
		}
	}

	void FRenderScene::BuildResource(const std::string& MapName)
	{	
		ClearRenderScene();
		RHIPtr = FRHIManager::GetInstance().GetRHIPtr();
		for (const auto& FileName : FileNames)
		{
			std::shared_ptr<FTexture> Tex = RHIPtr->CreateTexture(FileName);
			RHIPtr->CreateTextureResource(Tex.get());
			Textures.insert({ FileName, std::move(Tex) });
		}
		CreateDefaultShaders("Shaders\\color.hlsl", E_PSOTYPE::Default);
		CreateDefaultShaders("Shaders\\Shadows.hlsl", E_PSOTYPE::Shadow);
		CreateDefaultPsos();
		CreateDefaultMaterials();
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
				
#ifdef RENDER_PLATFORM_DX12
				std::shared_ptr<FRenderMesh> RenderMeshIns = RHIPtr->CreateRenderMesh();
				FDXRenderMesh* RenderMesh = dynamic_cast<FDXRenderMesh*>(RenderMeshIns.get());
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

				RHIPtr->CreateRenderMeshResource(RenderMesh);
				RenderMeshs.insert({ ActorMeshPrimitiveName , std::move(RenderMeshIns)});
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

		Textures.clear();

		for (auto& Shader : Shaders)
		{
			Shader.second = nullptr;
		}
		Shaders.clear();
		
		for (auto& Pso : Psos)
		{
			Pso.second.clear();
		}
		Psos.clear();
	}

	void FRenderScene::CreateBufferResources(const std::string& BufferName)
	{
		// if there is buffer exist, return;
		if (BufferResources.find(BufferName) != BufferResources.end())
		{
			return;
		}
		const auto& Iter = FMeshAsset::GetInstance().GetMeshInfors().find(BufferName);
		if (Iter == FMeshAsset::GetInstance().GetMeshInfors().end())
		{
			return;
		}
		const Charalotte::FMeshPrimitive& MeshPri = Iter->second;
		std::string Name = BufferName;
		if (MeshPri.LodInfos.size() <= 0)
		{
			std::stringstream ss;
			ss << BufferName << "No result";
			OutputDebugStringA(ss.str().c_str());
			return;
		}
		
		std::shared_ptr<Charalotte::FDXVerticesAndIndicesBuffer> VBIBBuffer = std::make_shared<Charalotte::FDXVerticesAndIndicesBuffer>();
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
		BufferResources.insert({BufferName, std::move(VBIBBuffer)});
	}

	void FRenderScene::CreateDefaultMaterials()
	{	
		std::string DefaultNormalPath = "Content/Textures/StoneNormal.dds";
		const auto& Iter = Textures.find(DefaultNormalPath);
		FTexture* DefaultNormal = nullptr;
		if (Iter != Textures.end())
		{
			DefaultNormal = Iter->second.get();
		}
		for (auto& Texture : Textures)
		{
			FMaterialAttributes DefAttribute;
			DefAttribute.BaseColor = Texture.second.get();
			DefAttribute.Normal = DefaultNormal;
			DefAttribute.Roughness = 1.0f;
			std::shared_ptr<FMaterial> Mat = std::make_shared<FMaterial>();
			Mat->SetMaterialAttributes(DefAttribute);
			Mat->AddPSO(GetPsoPtr(Default, "DefaultPso"), "DefaultPso", E_PSOTYPE::Default);
			Mat->AddPSO(GetPsoPtr(Shadow, "ShadowPso"), "ShadowPso", E_PSOTYPE::Shadow);
			Materials.insert({Texture.first, std::move(Mat)});
		}
	}

	void FRenderScene::CreateDefaultShaders(const std::string& Path, E_PSOTYPE PsoType)
	{
		std::shared_ptr<FShader> Shader;
		switch(PsoType)
		{
			case Default:
				Shader = std::make_shared<FBassPassShader>(Path);
				break;
			case Shadow:
				Shader = std::make_shared<FShadowShader>(Path);
				break;
		}
			
		std::shared_ptr<FVSPSAttributes> Att = std::make_shared<FVSPSAttributes>();
		Att->PSShaderMacroPtr = nullptr;
		Att->VSShaderMacroPtr = nullptr;
		Att->VSShaderVersion = "vs_5_0";
		Att->PSShaderVersion = "ps_5_0";
		Att->InputLayout =
		{
			{ "POSITION", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT , 0, 0, E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{ "COLOR", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 12,  E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
			{ "NORMAL", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 28,  E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
			{ "TEXCOORD", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32_FLOAT, 0, 44,  E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};
		Shader->SetAttributes(Att);
		RHIPtr->SetShaderElement(Shader.get());
		Shaders.insert({Path, std::move(Shader)});
	}
	void FRenderScene::CreatePso(E_PSOTYPE PsoType, const std::string& ShaderPath, const std::string& Name, FPSOAttributes PsoAtt)
	{
		auto ShaderIter = Shaders.find(ShaderPath);
		if (ShaderIter != Shaders.end())
		{
			std::shared_ptr<FRenderPSO> RenderPso = RHIPtr->CreatePSO(PsoAtt, ShaderIter->second.get());
			Psos[PsoType].insert({ Name , std::move(RenderPso)});
		}
	}

	void FRenderScene::CreateDefaultPsos()
	{
		FPSOAttributes DefaultPsoAtt;
		DefaultPsoAtt.IsUseBias = false;
		DefaultPsoAtt.NumRenderTargets = 1;
		DefaultPsoAtt.RTVFormat = E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM;
		CreatePso(E_PSOTYPE::Default, "Shaders\\color.hlsl", "DefaultPso", DefaultPsoAtt);

		FPSOAttributes ShadowPsoAtt;
		ShadowPsoAtt.IsUseBias = true;
		ShadowPsoAtt.DepthBias = 15000;
		ShadowPsoAtt.DepthBiasClamp = 0.0f;
		ShadowPsoAtt.SlopeScaledDepthBias = 1.0f;
		ShadowPsoAtt.NumRenderTargets = 0;
		ShadowPsoAtt.RTVFormat = E_GRAPHIC_FORMAT::FORMAT_UNKNOWN;
		CreatePso(E_PSOTYPE::Shadow, "Shaders\\Shadows.hlsl", "ShadowPso", ShadowPsoAtt);

	}

	FRenderPSO* FRenderScene::GetPsoPtr(E_PSOTYPE PsoType, const std::string& PsoName)
	{
		const auto& TypeIter = Psos.find(PsoType);
		if (TypeIter != Psos.end())
		{
			const auto& PsoIter = TypeIter->second.find(PsoName);
			if (PsoIter != TypeIter->second.end())
			{
				return PsoIter->second.get();
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			return nullptr;
		}
	}
}


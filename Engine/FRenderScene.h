#pragma once
#include <unordered_map>
#include "STransformDef.h"
#include "FDXVerticesAndIndicesBuffer.h"
#include "FShader.h"
#include "FRenderPSO.h"


namespace Charalotte
{
	class RHI;
	class FTexture;
	class FDXRenderMesh;
	class FRenderScene
	{
	public:
		FRenderScene();
		~FRenderScene() {
			ClearRenderScene();
		}

		void BuildResource(const std::string& MapName);
		
		void ClearRenderScene();

		inline std::unordered_map<std::string, std::shared_ptr<FRenderMesh>>& GetRenderMeshs()
		{
			return RenderMeshs;
		}

		void AddNewTexture(std::string TexturePath);

		void EraseTexture(std::string TexturePath);
	protected:
		void CreateBufferResources(const std::string& BufferName);
		void CreateRenderMeshs(const Charalotte::FActorPrimitive& ActorPrimitive);
		void CreateDefaultMaterials();
		void CreateDefaultShaders(const std::string&, E_PSOTYPE);
		void CreatePso(E_PSOTYPE PsoType, const std::string& ShaderPath, const std::string& PsoName, FPSOAttributes);
		void CreateDefaultPsos();
		FRenderPSO* GetPsoPtr(E_PSOTYPE PsoType, const std::string& PsoName);
	private:
		std::unordered_map<std::string, std::shared_ptr<FRenderMesh>> RenderMeshs;
		std::unordered_map<std::string, std::shared_ptr<FDXVerticesAndIndicesBuffer>> BufferResources;
		std::unordered_map<std::string, std::shared_ptr<FMaterial>> Materials;
		std::unordered_map<std::string, std::shared_ptr<FTexture>> Textures;
		std::unordered_map<std::string, std::shared_ptr<FShader>> Shaders;
		std::unordered_map<E_PSOTYPE, std::unordered_map<std::string, std::shared_ptr<FRenderPSO>>> Psos;
		std::vector<std::string> FileNames;
		RHI* RHIPtr;
	};
}

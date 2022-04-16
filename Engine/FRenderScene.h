#pragma once
#include <unordered_map>
#include "STransformDef.h"
#include "FRenderMesh.h"
#include "FVerticesAndIndicesBuffer.h"

class RHI;
namespace Charalotte
{
	class FTexture;
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
	private:
		std::unordered_map<std::string, std::shared_ptr<FRenderMesh>> RenderMeshs;
		std::unordered_map<std::string, std::shared_ptr<FVerticesAndIndicesBuffer>> BufferResources;
		std::unordered_map<std::string, std::shared_ptr<FMaterial>> Materials;
		std::unordered_map<std::string, std::shared_ptr<FTexture>> Textures;
		std::vector<std::string> FileNames;
		RHI* RHIPtr;
	};
}

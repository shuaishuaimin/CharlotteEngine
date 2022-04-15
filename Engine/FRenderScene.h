#pragma once
#include <unordered_map>
#include "STransformDef.h"
#include "FRenderMesh.h"
#include "FVerticesAndIndicesBuffer.h"

namespace Charalotte
{
	class FRenderScene
	{
	public:
		FRenderScene() {}
		~FRenderScene() {
			ClearRenderScene();
		}

		void BuildResource(const std::string& MapName);
		
		void ClearRenderScene();

		inline std::unordered_map<std::string, std::shared_ptr<FRenderMesh>>& GetRenderMeshs()
		{
			return RenderMeshs;
		}
	protected:
		void CreateBufferResources(const std::string& BufferName);
		void CreateRenderMeshs(const Charalotte::FActorPrimitive& ActorPrimitive);
	private:
		std::unordered_map<std::string, std::shared_ptr<FRenderMesh>> RenderMeshs;
		std::unordered_map<std::string, std::shared_ptr<FVerticesAndIndicesBuffer>> BufferResources;
		std::unordered_map<std::string, std::shared_ptr<FMaterial>> Materials;
	};
}

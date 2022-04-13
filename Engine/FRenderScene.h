#pragma once
#include <unordered_map>
#include "STransformDef.h"
#include "FScene.h"
#include "FRenderMesh.h"
#include "FScene.h"

namespace Charalotte
{
	class FRenderScene
	{
	public:
		FRenderScene() {}
		~FRenderScene() {}

		void BuildResource()
		{
			for (const auto& Actor : FScene::GetInstance().GetActorInfos())
			{
				
			}
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<FRenderMesh>> RenderMeshs;
	};
}

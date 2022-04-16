#pragma once
#include "FWindow.h"
#include "FRenderer.h"
#include "Singleton.h"
#include "FRenderScene.h"

namespace Charalotte
{
	class CharalotteEngine : public Singleton<CharalotteEngine>
	{

	public:
		CharalotteEngine();

		~CharalotteEngine() {}

		bool Init();

		int Update();

		std::shared_ptr<FWindow> CreateMainWindow();

		void tick();

		void Destory();

		FGameTimer* GetTimer();

		FWindow* GetWindowPtr();

		FRenderer* GetRenderPtr();

		FTempRenderScene* GetRenderScenePtr();

		FRenderScene* GetRenderSceneFinalButNotNow();

		std::vector<std::string>& GetTextureArray();

	private:
		std::unique_ptr<FRenderer> RenderIns;
		std::unique_ptr<RHI> RHIIns;
		std::shared_ptr<FWindow> WindowIns;
		std::unique_ptr<FGameTimer> Timer;
		std::vector<std::string> TextureArray;
		std::unique_ptr<FTempRenderScene> RenderScene;
		std::unique_ptr<FRenderScene> RealRenderScene;
	};

}


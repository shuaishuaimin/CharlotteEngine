#pragma once
#include "FWindow.h"
#include "FRender.h"
#include "Singleton.h"
#include "FRenderScene.h"

class CharalotteEngine : public Singleton<CharalotteEngine>
{

public:
	CharalotteEngine();

	~CharalotteEngine(){}

	bool Init();

	int Update();

	std::shared_ptr<FWindow> CreateMainWindow();

	void tick();

	void Destory();

	FGameTimer* GetTimer();

	FWindow* GetWindowPtr();

	FRender* GetRenderPtr();

	FRenderScene* GetRenderScenePtr();

	std::vector<std::string>& GetTextureArray();

private:
	std::unique_ptr<FRender> RenderIns;
	std::shared_ptr<FWindow> WindowIns;
	std::unique_ptr<FGameTimer> Timer;
	std::vector<std::string> TextureArray;
	std::unique_ptr<FRenderScene> RenderScene;
};

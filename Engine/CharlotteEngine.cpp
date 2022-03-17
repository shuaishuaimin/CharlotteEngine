#include "stdafx.h"
#include "CharlotteEngine.h"
#include "FScene.h"
#include "thread"


CharalotteEngine::CharalotteEngine() {
	WindowIns = this->CreateMainWindow();
#if PLATFORM_WINDOWS
	RenderIns = std::make_unique<DXRender>();
#endif
	Timer = std::make_unique<FGameTimer>();
	TextureArray = {"bricks", "bricks2", "bricks3", "grass", "ice", "stone", "tile", "WireFence", "WoodCrate01"};
}

bool CharalotteEngine::Init() {
	if (!WindowIns->InitMainWindow())
	{
		return false;
	}
	if (!RenderIns->Initialize())
	{
		return false;
	}
	return true;
}

std::shared_ptr<FWindow> CharalotteEngine::CreateMainWindow()
{
#if	PLATFORM_WINDOWS
	return std::make_shared<FWin32Window>();
#else
	return nullptr;
#endif
}

int CharalotteEngine::Update() {
	//std::thread 
#if PLATFORM_WINDOWS
	MSG msg = { 0 };

	Timer->Reset();

	// if message is not wm_quit. Refresh the window
	while (!WindowIns->GetIsExit())
	{
		WindowIns->Update();
		if (!WindowIns->GetIsPaused())
		{
			FScene::GetInstance().Update();
			RenderIns->Update();
		}
		
	}
	return (int)0;
#else
	return 0;
#endif
}

void CharalotteEngine::Destory()
{
	Sleep(1000);
	WindowIns = nullptr;

	RenderIns = nullptr;
}

void CharalotteEngine::tick()
{
	if (!WindowIns->GetIsPaused())
	{
		FScene::GetInstance().Update();
		RenderIns->Update();
	}
}

FGameTimer* CharalotteEngine::GetTimer()
{
	return Timer.get();
}

FWindow* CharalotteEngine::GetWindowPtr()
{
	return WindowIns.get();
}

FRender* CharalotteEngine::GetRenderPtr()
{
	return RenderIns.get();
}

std::vector<std::string>& CharalotteEngine::GetTextureArray()
{
	return TextureArray;
}
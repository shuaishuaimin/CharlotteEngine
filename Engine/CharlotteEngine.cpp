#include "stdafx.h"
#include "CharlotteEngine.h"
#include "FScene.h"
#include "thread"
#include "FGlobalDataManager.h"


CharalotteEngine::CharalotteEngine() {
	WindowIns = this->CreateMainWindow();
	RenderIns = std::make_unique<DXRender>();
	Timer = std::make_unique<FGameTimer>();
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
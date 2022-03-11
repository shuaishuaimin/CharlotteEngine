#include "stdafx.h"
#include "CharlotteEngine.h"
#include "FScene.h"
#include "thread"
#include "FGlobalDataManager.h"


CharalotteEngine::CharalotteEngine() {
	WindowIns = this->CreateMainWindow();
	RenderIns = std::make_unique<DXRender>();
}

bool CharalotteEngine::Init() {
	if (!WindowIns->InitMainWindow())
	{
		return false;
	}
	FGlobalDataManager::GetInstance().SaveWindowPtr(WindowIns.get());
	if (!RenderIns->Initialize())
	{
		return false;
	}
	return true;
}

std::shared_ptr<FWindow> CharalotteEngine::CreateMainWindow()
{
#if	PLATFORM_WINDOWS
	return std::make_shared<FWin32Window>(GetModuleHandle(0));
#else
	return nullptr;
#endif
}

int CharalotteEngine::Update() {
	//std::thread 
#if PLATFORM_WINDOWS
	MSG msg = { 0 };

	FGlobalDataManager::GetInstance().GetTimer()->Reset();

	// if message is not wm_quit. Refresh the window
	while (msg.message != WM_QUIT)
	{
		// if there are Window messages then process them
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// otherwise, do animation/game stuff
		else
		{
			FGlobalDataManager::GetInstance().GetTimer()->Tick();

			// if game pause sleep for wait
			// else calculate frame states and update timer, draw timer to screen
			if (!WindowIns->GetIsPaused())
			{
				WindowIns->Update();
				
			}
			else
			{
				Sleep(100);
			}
		}
		if (!WindowIns->GetIsPaused())
		{
			FScene::GetInstance().Update();
			RenderIns->Update();
		}
		
	}
	return (int)msg.wParam;
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
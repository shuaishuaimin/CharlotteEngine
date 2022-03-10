#include "stdafx.h"
#include "CharlotteEngine.h"
#include "FScene.h"
#include "thread"


CharalotteEngine::CharalotteEngine() {
	RenderIns = std::make_unique<DXRender>();
	WindowIns = RenderIns->CreateMainWindow();
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

int CharalotteEngine::Update() {
	//std::thread 
	MSG msg = { 0 };

	FScene::GetInstance().GetTimer()->Reset();

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
			FScene::GetInstance().GetTimer()->Tick();

			// if game pause sleep for wait
			// else calculate frame states and update timer, draw timer to screen
			if (!FScene::GetInstance().GetIsDXPaused())
			{
				WindowIns->Update();
				FScene::GetInstance().Update();
				RenderIns->Update();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}

void CharalotteEngine::Destory()
{
	Sleep(1000);
	WindowIns = nullptr;

	RenderIns = nullptr;
}
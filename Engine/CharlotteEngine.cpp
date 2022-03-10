#include "stdafx.h"
#include "CharlotteEngine.h"
#include "FSceneDataManager.h"
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

	FSceneDataManager::GetInstance().GetTimer()->Reset();

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
			FSceneDataManager::GetInstance().GetTimer()->Tick();

			// if game pause sleep for wait
			// else calculate frame states and update timer, draw timer to screen
			if (!FSceneDataManager::GetInstance().GetIsAppPaused())
			{
				WindowIns->Update();
				FSceneDataManager::GetInstance().Update();
				RenderIns->Update();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	//#if defined(DEBUG) || defined(_DEBUG)
	//{
	//	ID3D12DebugDevice* pDebugDevice = nullptr;
	//	md3dDevice->QueryInterface(&pDebugDevice);
	//	pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	//	pDebugDevice->Release();
	//}
	//#endif

	return (int)msg.wParam;
}

void CharalotteEngine::Destory()
{
	Sleep(1000);
	WindowIns = nullptr;

	RenderIns = nullptr;
}
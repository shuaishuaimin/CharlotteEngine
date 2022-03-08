#include "stdafx.h"
#include "CharlotteEngine.h"

#if PLATFORM_WINDOWS
CharalotteEngine::CharalotteEngine(HINSTANCE hInstance) {
	RenderIns = std::make_unique<DXRender>(hInstance);
	WindowIns = RenderIns->CreateMainWindow();
}
#else
CharalotteEngine::CharalotteEngine()
{
	RenderIns = std::make_unique<FApp>();
	WindowIns = RenderIns->CreateMainWindow();
}
#endif

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
	return RenderIns->Run();
}

void CharalotteEngine::Destory()
{
	Sleep(1000);
}
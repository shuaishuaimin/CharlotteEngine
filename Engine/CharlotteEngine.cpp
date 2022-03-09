#include "stdafx.h"
#include "CharlotteEngine.h"
#include "FScene.h"


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
	return RenderIns->Run();
}

void CharalotteEngine::Destory()
{
	Sleep(1000);
	WindowIns = nullptr;

	RenderIns = nullptr;
}
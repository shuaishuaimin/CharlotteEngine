#pragma once
#include "FWindow.h"
#include "FRender.h"
#include "FAssetSystem.h"
#if PLATFORM_WINDOWS
#include "DXRender.h"
#endif

class CharalotteEngine
{
public:
#if PLATFORM_WINDOWS
	CharalotteEngine(HINSTANCE hInstance);
#else
	CharalotteEngine();
#endif
	~CharalotteEngine(){}

	bool Init();

	int Update();

	void Destory();

private:
	std::unique_ptr<FRender> RenderIns;
	std::shared_ptr<FWindow> WindowIns;
};


#pragma once
#include "FWindow.h"
#include "FRender.h"
#include "Singleton.h"
#if PLATFORM_WINDOWS
#include "DXRender.h"
#endif

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

private:
	std::unique_ptr<FRender> RenderIns;
	std::shared_ptr<FWindow> WindowIns;
};


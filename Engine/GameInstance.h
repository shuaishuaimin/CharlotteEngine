#pragma once
#if PLATFORM_WINDOWS
#include <windows.h>
#endif
#include <memory>
#include "CharlotteEngine.h"
#include "GameLogic.h"

class GameInstance
{
public:
#if PLATFORM_WINDOWS
	GameInstance(HINSTANCE hInstance);
#else
	GameInstance();
#endif
	~GameInstance();

	bool Init();

	int Update();

	void Destory();

protected:
	std::unique_ptr<CharalotteEngine> Engine;
	std::unique_ptr<GameLogic> Game;
};


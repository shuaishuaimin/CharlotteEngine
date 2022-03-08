#include "stdafx.h"
#include "GameInstance.h"
#include "FScene.h"

#if PLATFORM_WINDOWS
GameInstance::GameInstance(HINSTANCE hInstance)
{
	Engine = std::make_unique<CharalotteEngine>(hInstance);
	Game = std::make_unique<GameLogic>();
}
#else
GameInstance::GameInstance()
{
	Engine = std::make_unique<CharalotteEngine>();
	Game = std::make_unique<GameLogic>();
}
#endif

bool GameInstance::Init()
{
	FScene::GetInstance().GetAssetSystem()->LoadMap("ThirdPersonExampleMap.dat");
	if (!Engine->Init())
	{
		return false;
	}
	Game->Init();
	return true;
}

int GameInstance::Update()
{
	int Result = Engine->Update();
	Game->Update();
	return Result;
}

void GameInstance::Destory()
{
	Engine->Destory();
	Game->Destory();
}


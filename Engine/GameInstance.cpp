#include "stdafx.h"
#include "GameInstance.h"
#include "FScene.h"
#include <thread>

GameInstance::GameInstance()
{
	Engine = std::make_unique<CharalotteEngine>();
	Game = std::make_unique<GameLogic>();
}

GameInstance::~GameInstance()
{
	
}

bool GameInstance::Init()
{
	if (!Engine->Init())
	{
		return false;
	}
	Game->Init();
	return true;
}

int GameInstance::Update()
{
	/*std::thread t1(&CharalotteEngine::Update, Engine.get());
	std::thread t2(&GameLogic::Update, Game.get());
	t1.join();
	t1.join();*/
	int Result = Engine->Update();
	Game->Update();
	return Result;
}

void GameInstance::Destory()
{
	Engine->Destory();
	Game->Destory();
}


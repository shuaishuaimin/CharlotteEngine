#include "stdafx.h"
#include "GameInstance.h"
#include "FScene.h"


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
	int Result = Engine->Update();
	Game->Update();
	return Result;
}

void GameInstance::Destory()
{
	Engine->Destory();
	Game->Destory();
}


#include "stdafx.h"
#include "GameLogic.h"

void GameLogic::Init()
{
	FScene::GetInstance().LoadMap("ThirdPersonExampleMap.dat");
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('P', [](){
		FScene::GetInstance().LoadMap("Cool");
		});
}
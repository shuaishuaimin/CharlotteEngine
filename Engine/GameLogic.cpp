#include "stdafx.h"
#include "GameLogic.h"

void GameLogic::Init()
{
	Charalotte::FScene::GetInstance().LoadMap("ThirdPersonExampleMap.dat");
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('P', [](){
		Charalotte::FScene::GetInstance().LoadMap("Cool.dat");
		});
	FWinEventRegisterSystem::GetInstance().RegisterKeyInput('L', []() {
		Charalotte::FScene::GetInstance().LoadMap("ThirdPersonExampleMap.dat");
		});
}
#pragma once
#include "stdafx.h"
#include "FWinEventRegisterSystem.h"
#include "FGameTimer.h"
#include "FSceneDataManager.h"

class GameLogic
{
public:
	GameLogic(){};
	~GameLogic(){};

	void Init()
	{
		FSceneDataManager::GetInstance().LoadMap("ThirdPersonExampleMap.dat");
	}

	void Update(){}

	void Destory(){}
};

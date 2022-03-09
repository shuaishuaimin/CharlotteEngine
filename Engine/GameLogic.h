#pragma once
#include "stdafx.h"
#include "FWinEventRegisterSystem.h"
#include "FGameTimer.h"
#include "FScene.h"

class GameLogic
{
public:
	GameLogic(){};
	~GameLogic(){};

	void Init()
	{
		FScene::GetInstance().GetAssetSystem()->LoadMap("ThirdPersonExampleMap.dat");
	}

	void Update(){}

	void Destory(){}
};

#pragma once
#include "stdafx.h"
#include "FWinInputSystem.h"
#include "FGameTimer.h"
#include "FScene.h"

class GameLogic
{
public:
	GameLogic(){};
	~GameLogic(){};
	// register mouse and keyboard event
	void RegisterEvent();

	void Init()
	{
		FScene::GetInstance().GetAssetSystem()->LoadMap("ThirdPersonExampleMap.dat");
	}

	void Update(){}

	void Destory(){}
};

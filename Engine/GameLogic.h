#pragma once
#include "stdafx.h"
#include "FWinInputSystem.h"
#include "FGameTimer.h"
#include "FScene.h"

class GameLogic
{
public:
	GameLogic(){};
	// register mouse and keyboard event
	void RegisterEvent();

	void Init()
	{
		RegisterEvent();
	}

	void Update(){}

	void Destory(){}
};

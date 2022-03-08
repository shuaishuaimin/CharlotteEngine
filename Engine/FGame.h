#pragma once
#include "stdafx.h"
#include "FWinInputSystem.h"
#include "FGameTimer.h"
#include "FScene.h"

class FGame
{
public:
	FGame(){};
	// register mouse and keyboard event
	void RegisterEvent();

	void Init()
	{
		RegisterEvent();
	}

	void Run(){}
};

#pragma once
#include <memory>
#include "CharlotteEngine.h"
#include "GameLogic.h"

class GameInstance
{
public:
	GameInstance();
	~GameInstance();

	bool Init();

	int Update();

	void Destory();

protected:
	std::unique_ptr<GameLogic> Game;
};


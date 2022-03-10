#include "stdafx.h"
#include "FGlobalDataManager.h"

FGlobalDataManager::FGlobalDataManager()
{
	GameTimer = std::make_unique<FGameTimer>();
	IsDeviceSucceed = false;
}
FGlobalDataManager::~FGlobalDataManager()
{
	GameTimer = nullptr;
}

FGameTimer* FGlobalDataManager::GetTimer()
{
	return GameTimer.get();
}

bool FGlobalDataManager::GetIsDeviceSucceed()
{
	return IsDeviceSucceed;
}

void FGlobalDataManager::SetIsDeviceSucceed(bool IsSucceed)
{
	IsDeviceSucceed = IsSucceed;
}

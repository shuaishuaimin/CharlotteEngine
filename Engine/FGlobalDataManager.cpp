#include "stdafx.h"
#include "FGlobalDataManager.h"

FGlobalDataManager::FGlobalDataManager()
{
	GameTimer = std::make_unique<FGameTimer>();
	IsDXPaused = false;
	IsDeviceSucceed = false;
	IsCanResizing = false;
}
FGlobalDataManager::~FGlobalDataManager()
{
	GameTimer = nullptr;
}


FGameTimer* FGlobalDataManager::GetTimer()
{
	return GameTimer.get();
}

bool FGlobalDataManager::GetIsDXPaused()
{
	return IsDXPaused;
}

void FGlobalDataManager::SetDXPaused(bool IsPaused)
{
	IsDXPaused = IsPaused;
}

bool FGlobalDataManager::GetIsDeviceSucceed()
{
	return IsDeviceSucceed;
}

void FGlobalDataManager::SetIsDeviceSucceed(bool IsSucceed)
{
	IsDeviceSucceed = IsSucceed;
}

bool FGlobalDataManager::GetIsCanResizing()
{
	return IsCanResizing;
}

void FGlobalDataManager::SetIsCanResizing(bool IsCan)
{
	IsCanResizing = IsCan;
}


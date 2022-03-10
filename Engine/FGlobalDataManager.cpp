#include "stdafx.h"
#include "FGlobalDataManager.h"

FGlobalDataManager::FGlobalDataManager()
{
	GameTimer = std::make_unique<FGameTimer>();
	IsDeviceSucceed = false;
	NowWindow = nullptr;
}
FGlobalDataManager::~FGlobalDataManager()
{
	GameTimer = nullptr;
	NowWindow = nullptr;
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
void FGlobalDataManager::SaveWindowPtr(FWindow* WindowPtr)
{
	NowWindow = WindowPtr;
}

FWindow* FGlobalDataManager::GetWindowPtr()
{
	return NowWindow;
}
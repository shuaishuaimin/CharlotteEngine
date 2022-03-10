#pragma once
#include "Singleton.h"

class FGlobalDataManager : public Singleton<FGlobalDataManager>
{
public:
	FGlobalDataManager();
	~FGlobalDataManager();

	FGameTimer* GetTimer();

	bool GetIsDeviceSucceed();

	void SetIsDeviceSucceed(bool IsSucceed);

	void SaveWindowPtr(FWindow* WindowPtr); 

	FWindow* GetWindowPtr();

private:
	std::unique_ptr<FGameTimer> GameTimer;

	bool IsDeviceSucceed;

	FWindow* NowWindow;
};


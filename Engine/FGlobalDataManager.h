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

	bool GetIsCanResizing();

	void SetIsCanResizing(bool IsCan);

private:
	std::unique_ptr<FGameTimer> GameTimer;

	bool IsDeviceSucceed;

	bool IsCanResizing;
};


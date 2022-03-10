#pragma once
#include "Singleton.h"

class FGlobalDataManager : public Singleton<FGlobalDataManager>
{
public:
	FGlobalDataManager();
	~FGlobalDataManager();

	FGameTimer* GetTimer();

	bool GetIsDXPaused();

	void SetDXPaused(bool IsPaused);

	bool GetIsDeviceSucceed();

	void SetIsDeviceSucceed(bool IsSucceed);

	bool GetIsCanResizing();

	void SetIsCanResizing(bool IsCan);

private:
	std::unique_ptr<FGameTimer> GameTimer;
	// connect app and window
	bool IsDXPaused;

	bool IsDeviceSucceed;

	bool IsCanResizing;
};


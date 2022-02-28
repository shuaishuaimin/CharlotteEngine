#pragma once
#include "FWindow.h"

class FApp
{
public:
	FApp() {}
	virtual FWindow* CreateMainWindow() = 0;
	virtual FApp* GetOwnApp() = 0;
	virtual void Init() = 0;
};
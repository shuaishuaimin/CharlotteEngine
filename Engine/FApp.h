#pragma once
#include <iostream>
#include "FWindow.h"

class FApp
{
public:
	FApp() {}
	virtual std::shared_ptr<FWindow> CreateMainWindow() = 0;
	virtual FApp* GetOwnApp() = 0;
	virtual bool Initialize() = 0;
	virtual int Run() = 0;
};
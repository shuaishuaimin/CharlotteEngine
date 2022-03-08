#pragma once
#include <iostream>
#include "FWindow.h"

class FRender
{
public:
	FRender() {}
	virtual ~FRender(){}
	virtual std::shared_ptr<FWindow> CreateMainWindow() = 0;
	virtual FRender* GetOwnApp() = 0;
	virtual bool Initialize() = 0;
	virtual int Run() = 0;
};
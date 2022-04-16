#pragma once
#include <iostream>
#include "FWindow.h"

class FRenderer
{
public:
	FRenderer() {}
	virtual ~FRenderer(){}
	virtual bool Initialize() = 0;
	virtual void Update() = 0;
	virtual void StoreRHIPtr() = 0;
	virtual bool GetIsDevicedSucceed() = 0;
};
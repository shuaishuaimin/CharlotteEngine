#pragma once
#include <iostream>
#include "FWindow.h"

class FRender
{
public:
	FRender() {}
	virtual ~FRender(){}
	virtual bool Initialize() = 0;
	virtual void Update() = 0;
};
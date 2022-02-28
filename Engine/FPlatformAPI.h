#pragma once
#include <iostream>
#include "FApp.h"

class FPlatformAPI
{
public:
	FPlatformAPI(){}
	~FPlatformAPI(){}

	static int InitResult(std::shared_ptr<FApp> App);
};
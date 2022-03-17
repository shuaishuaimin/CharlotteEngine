#pragma once

#include "Singleton.h"
#include "string"

class RHI
{
public:
	RHI() {}
	virtual ~RHI() {}
	virtual void LoadTextureAsset(const std::string& FileName) = 0;

};
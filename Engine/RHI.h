#pragma once

#include "string"
#include "FWindow.h"

class RHI
{
public:
	RHI() {}
	virtual ~RHI() {}

	virtual bool InitRenderPlatform(FWindow* WindowPtr) = 0;
	virtual bool InitRenderPipeline() = 0;
	virtual void LoadTextureAsset(const std::string& FileName, const std::string& FilePath) = 0;
	virtual void OnResize() = 0;

};
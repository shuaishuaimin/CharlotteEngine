#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "FUtil.h"
#include "FRender.h"
#include "FGameTimer.h"
#include "FWin32Window.h"
#include "FMathHelper.h"
#include "FUploadBuffer.h"
#include "DXPrimitives.h"
#include "FCamera.h"
#include "FWinEventRegisterSystem.h"
#include "FScene.h"
#include "WinMaterialData.h"
#include "RHI.h"
// link necessary lib
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class DXRender : public FRender
{
public:
	DXRender();
	// ban copy constructor and assignment
	DXRender(const DXRender& dm) = delete;
	DXRender operator= (const DXRender& dm) = delete;
	virtual ~DXRender();

	virtual bool Initialize()override;

	void LoadingMapDataFromAssetSystem(const std::string& MapName);

	void Destory(){};

	static DXRender* GetRender();

	void Update() override;

	virtual bool GetIsDevicedSucceed()override;
private:

	static DXRender* render;

	std::unique_ptr<RHI> RHIIns;
};
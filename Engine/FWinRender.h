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

class FWinRender : public FRender
{
public:
	FWinRender();
	// ban copy constructor and assignment
	FWinRender(const FWinRender& dm) = delete;
	FWinRender operator= (const FWinRender& dm) = delete;
	virtual ~FWinRender();

	virtual bool Initialize()override;

	void LoadingMapDataFromAssetSystem(const std::string& MapName);

	void Destory(){};

	void Update() override;

	virtual bool GetIsDevicedSucceed()override;
private:

	std::unique_ptr<RHI> RHIIns;

	std::shared_ptr<Charalotte::DrawNecessaryData> DrawData;
};
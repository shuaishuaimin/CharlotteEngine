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
//#pragma comment(lib,"d3dcompiler.lib")
//#pragma comment(lib, "D3D12.lib")
//#pragma comment(lib, "dxgi.lib")

class FPCRender : public FRender
{
public:
	FPCRender();
	// ban copy constructor and assignment
	FPCRender(const FPCRender& dm) = delete;
	FPCRender operator= (const FPCRender& dm) = delete;
	virtual ~FPCRender();

	virtual bool Initialize()override;

	void InitLight();
	void LoadingMapDataFromAssetSystem(const std::string& MapName);

	void Destory(){};

	void Update() override;

	virtual bool GetIsDevicedSucceed()override;

protected:
	void UpdateShadowPassCB();

	void UpdateShadowCons(Charalotte::ObjectConstants& Obj, const Charalotte::FActorInfo& ActorPri);

	void UpDateCommonCons(Charalotte::ObjectConstants& Obj, const Charalotte::FActorInfo& ActorPri);

	void BuildCommonInputLayout();


	void BuildShadowInputLayout();
private:

	std::unique_ptr<RHI> RHIIns;

	std::shared_ptr<Charalotte::DrawNecessaryData> DrawData;

	std::shared_ptr<Charalotte::DrawNecessaryData> TestLightData;

	std::string NowMapName;

	// test
	glm::mat4 mLightView = glm::mat4(1.0f);
	glm::mat4 mLightProj = glm::mat4(1.0f);

	std::shared_ptr<Charalotte::FShaderInput> CommonShaderInput;
	std::shared_ptr<Charalotte::FShaderInput> ShadowShaderInput;
};
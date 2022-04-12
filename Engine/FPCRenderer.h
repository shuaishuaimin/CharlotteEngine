#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "FUtil.h"
#include "FRenderer.h"
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

namespace Charalotte
{
	class FPCRenderer : public FRenderer
	{
	public:
		FPCRenderer();
		// ban copy constructor and assignment
		FPCRenderer(const FPCRenderer& dm) = delete;
		FPCRenderer operator= (const FPCRenderer& dm) = delete;
		virtual ~FPCRenderer();

		virtual bool Initialize()override;

		void InitLight();
		void LoadingMapDataFromAssetSystem(const std::string& MapName);

		void Destory() {};

		void Update() override;

		virtual bool GetIsDevicedSucceed()override;

	protected:
		void GetRHI();
		
		void UpdateShadowPassCB();

		void UpdateShadowCons(ObjectConstants& Obj, const FActorInfo& ActorPri);

		void UpDateCommonCons(ObjectConstants& Obj, const FActorInfo& ActorPri);

		void BuildCommonInputLayout();


		void BuildShadowInputLayout();
	private:

		RHI* RHIPtr;

		std::shared_ptr<RenderUsefulData> DrawData;

		std::shared_ptr<RenderUsefulData> TestLightData;

		std::string NowMapName;

		// test
		glm::mat4 mLightView = glm::mat4(1.0f);
		glm::mat4 mLightProj = glm::mat4(1.0f);
		glm::vec3 mNowLightPos = glm::vec3(1.0f);

		std::shared_ptr<FShaderInfo> CommonShaderInput;
		std::shared_ptr<FShaderInfo> ShadowShaderInput;
	};
}

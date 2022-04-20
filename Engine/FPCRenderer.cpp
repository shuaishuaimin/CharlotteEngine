#include "stdafx.h"
#include <DirectXColors.h>
#include <cstdint>
#include <WindowsX.h>
#include <string>
#include "FPCRenderer.h"
#include "FDataProcessor.h"
#include "FWinRenderScene.h"
#include "CharlotteEngine.h"
#include "FMeshAsset.h"
#include "RHIResource.h"
#include "SRHIConstants.h "
#include "SEShaderElements.h"
#include "CharlotteEngine.h"
#ifdef RENDER_PLATFORM_DX12
#include "DX12RHI.h"
#endif
#include "FRHIManager.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace DirectX::PackedVector;
using std::string;

namespace Charalotte
{
	FPCRenderer::FPCRenderer()
	{
		
	}


	FPCRenderer::~FPCRenderer()
	{

	}

	void FPCRenderer::StoreRHIPtr()
	{
		RHIPtr = FRHIManager::GetInstance().GetRHIPtr();
		FWinEventRegisterSystem::GetInstance().RegisterOnResize(DXRenderResize, [this]() {
			this->RHIPtr->OnResize();
			});
		FWinEventRegisterSystem::GetInstance().RegisterMapLoadEventForDender(MapLoadType::BaseMapLoad, [this](const std::string& MapName) {
			this->LoadingMapDataFromAssetSystem(MapName);
			});

		DrawData = std::make_shared<RenderUsefulData>();
		TestLightData = std::make_shared<RenderUsefulData>();
		CommonShaderInput = std::make_shared<FShaderInfo>();
		ShadowShaderInput = std::make_shared<FShaderInfo>();
		ShadowMapRT = std::make_shared<FPCRenderTarget>();
		BasePassRT = std::make_shared<FPCRenderTarget>();
	}

	bool FPCRenderer::Initialize()
	{
		if (!RHIPtr->InitRenderPlatform(CharalotteEngine::GetInstance().GetWindowPtr()))
		{
			return false;
		}
		RHIPtr->OnResize();
		BuildCommonInputLayout();
		BuildShadowInputLayout();
		RHIPtr->BuildRootSignature(Default);
		RHIPtr->BuildPSO();
		RHIPtr->BuildRootSignature(Shadow);
		RHIPtr->BuildShadowPSO();
		RHIPtr->InitShadowMap();
		TestCreateAndUseRT();
		RHIPtr->FlushCommandQueue();
		ShadowMap = RHIPtr->CreateShadowMap();
		return true;

	}

	void FPCRenderer::InitLight()
	{
		TestLightData->VPTransform.VPMatrix = mLightProj * mLightView;
		TestLightData->VPTransform.ProjectionTransform = mLightProj;
		TestLightData->VPTransform.ViewTransform = mLightView;
	}
	// draw by camera data
	void FPCRenderer::Update()
	{
		RHIPtr->OpenCommandList(true);
		UpdateShadowPassCB();
		InitLight();
		FScene::GetInstance().GetCamera()->GetCameraData(DrawData->MainCameraData);
		FScene::GetInstance().GetCamera()->GetVPTransform(DrawData->VPTransform.VPMatrix);
		const auto& Actors = FScene::GetInstance().GetActorInfos();
		const auto& ActorIter = Actors.find(NowMapName);


		if (ActorIter != Actors.end())
		{
			// draw shadow
			RHIPtr->DrawPrepare(Shadow);
			for (const auto& ActorPri : ActorIter->second.ActorsInfo)
			{
				ObjectConstants objConstants;
				UpdateShadowCons(objConstants, ActorPri);
				RHIPtr->SetPipelineParamter(Shadow, ActorPri,
					TestLightData.get(), CharalotteEngine::GetInstance().GetRenderScenePtr());
				RHIPtr->DrawMesh(ActorPri, TestLightData.get(), objConstants, CharalotteEngine::GetInstance().GetRenderScenePtr());
			}
			//RHIIns->DrawEnd(Shadow);
			RHIPtr->ExecuteAndCloseCommandList();
			RHIPtr->FlushCommandQueue();

			RHIPtr->OpenCommandList(true);
			// draw actor
			RHIPtr->DrawPrepare(Default);
			for (const auto& ActorPri : ActorIter->second.ActorsInfo)
			{
				ObjectConstants objConstants;
				UpDateCommonCons(objConstants, ActorPri);
				RHIPtr->SetPipelineParamter(Default, ActorPri,
					DrawData.get(), CharalotteEngine::GetInstance().GetRenderScenePtr());
				RHIPtr->DrawMesh(ActorPri, DrawData.get(), objConstants, CharalotteEngine::GetInstance().GetRenderScenePtr());
			}
			RHIPtr->DrawEnd(Default);
			RHIPtr->ExecuteAndCloseCommandList();
			RHIPtr->SwapChain();
			RHIPtr->FlushCommandQueue();
		}
	}

	bool FPCRenderer::GetIsDevicedSucceed()
	{
		return RHIPtr->GetIsDeviceSucceed();
	}

	void FPCRenderer::LoadingMapDataFromAssetSystem(const std::string& MapName)
	{
		NowMapName = MapName;
		const auto& ActorPrimitiveIter = FScene::GetInstance().GetActorInfos().find(MapName);
		auto Actors = FScene::GetInstance().GetActorInfos();
		if (ActorPrimitiveIter != FScene::GetInstance().GetActorInfos().end())
		{

			RHIPtr->BuildMeshAndActorPrimitives(ActorPrimitiveIter->second, FMeshAsset::GetInstance().GetMeshInfors(), CharalotteEngine::GetInstance().GetRenderScenePtr());

		}
		else
		{
			return;
		}
		// load texture
		for (const auto& TextureName : CharalotteEngine::GetInstance().GetTextureArray())
		{
			std::string TexturePath = "Content/Textures/" + TextureName + ".dds";
			RHIPtr->LoadTextureResource(TextureName, TexturePath, CharalotteEngine::GetInstance().GetRenderScenePtr());
		}

		// Build scene and compile material
		RHIPtr->BuildSceneResourcesForRenderPlatform(CharalotteEngine::GetInstance().GetRenderScenePtr());
		RHIPtr->CompileMaterial(CharalotteEngine::GetInstance().GetRenderScenePtr());
	}

	void FPCRenderer::UpdateShadowPassCB()
	{
		glm::vec3 lightPos = { -5000.0f,0.0f,2500.0f };
		glm::vec3 targetPos = { 1.0f,0.0f,-0.5f };
		glm::vec3 lightUp = glm::vec3(0.0f, 0.0f, 1.0f);

		//int Time = CharalotteEngine::GetInstance().GetTimer()->TotalTime();
		int Time = 0;
		lightPos = glm::vec4(lightPos, 0.0f) * glm::rotate(glm::mat4(1.0f), Time % 4 * glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		targetPos = glm::vec4(targetPos, 0.0f) * glm::rotate(glm::mat4(1.0f), Time % 4 * glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		mNowLightPos = lightPos + targetPos;
		glm::mat4 lightView = glm::lookAtLH(lightPos, lightPos + targetPos, lightUp);

		glm::vec3 LS = FMathHelper::Vector3TransformCoord(targetPos, lightView);

		float Radius = 2500;

		float l = LS.x - Radius;
		float b = LS.y - Radius;
		float n = LS.z - Radius;
		float r = LS.x + Radius;
		float t = LS.y + Radius;
		float f = LS.z + Radius;

		glm::mat4 lightProj = glm::orthoLH_ZO(l, r, b, t, n, f);

		mLightView = lightView;
		mLightProj = lightProj;
	}

	void FPCRenderer::UpdateShadowCons(ObjectConstants& Obj, const FActorInfo& ActorPri)
	{
		glm::mat4 NowWorldTrans = FMathHelper::GetWorldTransMatrix(ActorPri.Transform);
		Obj.Tans = glm::transpose(mLightProj * mLightView * NowWorldTrans);
	}

	void FPCRenderer::UpDateCommonCons(ObjectConstants& objConstants, const FActorInfo& ActorPri)
	{
		glm::mat4 NowVPTrans;
		FScene::GetInstance().GetCamera()->GetVPTransform(NowVPTrans);
		glm::mat4 T(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);

		auto& RotateStruct = ActorPri.Transform.Rotation;
		glm::vec4 Rotate(RotateStruct.X, RotateStruct.Y, RotateStruct.Z, RotateStruct.W);
		glm::mat4 NowRotate = FMathHelper::GetRotateMatrix(Rotate);
		glm::mat4 NowWorldTrans = FMathHelper::GetWorldTransMatrix(ActorPri.Transform);

		objConstants.Test = glm::transpose(mLightProj * mLightView);
		objConstants.Tans = glm::transpose(T * mLightProj * mLightView);
		objConstants.World = glm::transpose(NowWorldTrans);

		objConstants.MVP = glm::transpose(NowVPTrans * NowWorldTrans);
		objConstants.Scale3D = glm::transpose(FMathHelper::GetScaleMatrix(ActorPri.Transform));
		objConstants.Rotate = NowRotate;
		objConstants.LightInfo.LightStrength = 2.0f;
		objConstants.LightInfo.LightVec = glm::vec4(mNowLightPos, 1.0f);
		objConstants.Offset = CharalotteEngine::GetInstance().GetTimer()->TotalTime();
	}

	void FPCRenderer::BuildCommonInputLayout()
	{
		CommonShaderInput->PsoType = Default;
		CommonShaderInput->PSShaderMacroPtr = nullptr;
		CommonShaderInput->VSShaderMacroPtr = nullptr;
		CommonShaderInput->VSShaderVersion = "vs_5_0";
		CommonShaderInput->PSShaderVersion = "ps_5_0";
		CommonShaderInput->ShaderFilePath = "Shaders\\color.hlsl";
		CommonShaderInput->InputLayout =
		{
			{ "POSITION", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT , 0, 0, E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{ "COLOR", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 12,  E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
			{ "NORMAL", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 28,  E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
			{ "TEXCOORD", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32_FLOAT, 0, 44,  E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};
		RHIPtr->SetShader(CommonShaderInput);
	}

	void FPCRenderer::BuildShadowInputLayout()
	{
		ShadowShaderInput->PsoType = Shadow;
		ShadowShaderInput->PSShaderMacroPtr = nullptr;
		ShadowShaderInput->VSShaderMacroPtr = nullptr;
		ShadowShaderInput->VSShaderVersion = "vs_5_0";
		ShadowShaderInput->PSShaderVersion = "ps_5_0";
		ShadowShaderInput->ShaderFilePath = "Shaders\\Shadows.hlsl";
		ShadowShaderInput->InputLayout =
		{
			{ "POSITION", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT , 0, 0, E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{ "COLOR", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 12,  E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
			{ "NORMAL", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 28,  E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
			{ "TEXCOORD", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32_FLOAT, 0, 44,  E_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};
		RHIPtr->SetShader(ShadowShaderInput);
	}

	// re construct function
	void FPCRenderer::SceneRender()
	{	
		RHIPtr->BeginFrame();
		RenderShadowDepth();
		RenderBasePass();
	}

	void FPCRenderer::RenderShadowDepth()
	{
		glm::mat4 LightView;
		glm::mat4 LightProj;
		BuildLightMatrix(LightView, LightProj);
		RHIPtr->SetRenderTarget(ShadowMapRT.get());
		for (auto& RenderMesh : CharalotteEngine::GetInstance().GetRenderSceneFinalButNotNow()->GetRenderMeshs())
		{
			auto Pso = RenderMesh.second->GetMaterialPtr()->GetPso("ShadowPso", E_PSOTYPE::Shadow);
			RHIPtr->SetPSOFinal(Pso);
			Pso->GetShader()->SetRarameter(RHIPtr);
			RHIPtr->DrawMeshFinal(*TestLightData, RenderMesh.second.get());
			
		}
	}

	void FPCRenderer::RenderBasePass()
	{
		RHIPtr->SetRenderTarget(BasePassRT.get());
		for (auto& RenderMesh : CharalotteEngine::GetInstance().GetRenderSceneFinalButNotNow()->GetRenderMeshs())
		{
			auto Pso = RenderMesh.second->GetMaterialPtr()->GetPso("DefaultPso", E_PSOTYPE::Default);
		
			RHIPtr->SetPSOFinal(Pso);
			Pso->GetShader()->SetRarameter(RHIPtr);
			RHIPtr->DrawMeshFinal(*DrawData, RenderMesh.second.get());
		}
	}

	glm::mat4 FPCRenderer::BuildLightMatrix()
	{
		glm::vec3 lightPos = { -5000.0f,0.0f,2500.0f };
		glm::vec3 targetPos = { 1.0f,0.0f,-0.5f };
		glm::vec3 lightUp = glm::vec3(0.0f, 0.0f, 1.0f);

		//int Time = CharalotteEngine::GetInstance().GetTimer()->TotalTime();
		int Time = 0;
		lightPos = glm::vec4(lightPos, 0.0f) * glm::rotate(glm::mat4(1.0f), Time % 4 * glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		targetPos = glm::vec4(targetPos, 0.0f) * glm::rotate(glm::mat4(1.0f), Time % 4 * glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		mNowLightPos = lightPos + targetPos;
		glm::mat4 lightView = glm::lookAtLH(lightPos, lightPos + targetPos, lightUp);

		glm::vec3 LS = FMathHelper::Vector3TransformCoord(targetPos, lightView);

		float Radius = 2500;

		float l = LS.x - Radius;
		float b = LS.y - Radius;
		float n = LS.z - Radius;
		float r = LS.x + Radius;
		float t = LS.y + Radius;
		float f = LS.z + Radius;
		glm::mat4 lightProj = glm::orthoLH_ZO(l, r, b, t, n, f);
		return lightProj * lightView;
	}

	void FPCRenderer::BuildLightMatrix(glm::mat4& LightView, glm::mat4& LightProj)
	{
		glm::vec3 lightPos = { -5000.0f,0.0f,2500.0f };
		glm::vec3 targetPos = { 1.0f,0.0f,-0.5f };
		glm::vec3 lightUp = glm::vec3(0.0f, 0.0f, 1.0f);

		//int Time = CharalotteEngine::GetInstance().GetTimer()->TotalTime();
		int Time = 0;
		lightPos = glm::vec4(lightPos, 0.0f) * glm::rotate(glm::mat4(1.0f), Time % 4 * glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		targetPos = glm::vec4(targetPos, 0.0f) * glm::rotate(glm::mat4(1.0f), Time % 4 * glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		mNowLightPos = lightPos + targetPos;
		glm::mat4 lightView = glm::lookAtLH(lightPos, lightPos + targetPos, lightUp);

		glm::vec3 LS = FMathHelper::Vector3TransformCoord(targetPos, lightView);

		float Radius = 2500;

		float l = LS.x - Radius;
		float b = LS.y - Radius;
		float n = LS.z - Radius;
		float r = LS.x + Radius;
		float t = LS.y + Radius;
		float f = LS.z + Radius;
		glm::mat4 lightProj = glm::orthoLH_ZO(l, r, b, t, n, f);
		LightView = lightView;
		LightProj = lightProj;
	}

	void FPCRenderer::TestCreateAndUseRT()
	{
		FResourceElement El;
		El.DsDsvOffest = 0;
		El.DsRtvOffest = 0;
		El.RtDsvOffest = 0;
		El.RtRtvOffest = 0;
		std::shared_ptr<FPCRenderTarget> Rt = RHIPtr->CreateRenderTarget();
		FResourceAttributes Attribute;
		Attribute.DsO = 0;
		Attribute.RtO = 0;
		Attribute.SrO = 0;
		Attribute.Height = 1280;
		Attribute.Width = 1980;
		Attribute.Format = E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT;
		Attribute.ResourceType = E_RESOURCE_TYPE::RESOURCE_DEP;
		Rt->SetDSResource(RHIPtr->CreateResource(Attribute));
		Attribute.Format = E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT;
		Attribute.ResourceType = E_RESOURCE_TYPE::RESOURCE_RT;
		Rt->SetRTResource(RHIPtr->CreateResource(Attribute));
	}
}


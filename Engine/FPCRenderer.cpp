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

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace DirectX::PackedVector;
using std::string;

namespace Charalotte
{
	FPCRenderer::FPCRenderer()
	{
#ifdef RENDER_PLATFORM_DX12
		RHIIns = std::make_unique<DX12RHI>();
#endif
		FWinEventRegisterSystem::GetInstance().RegisterMapLoadEventForDender(MapLoadType::BaseMapLoad, [this](const std::string& MapName) {
			this->LoadingMapDataFromAssetSystem(MapName);
			});
		FWinEventRegisterSystem::GetInstance().RegisterOnResize(DXRenderResize, [this]() {
			this->RHIIns->OnResize();
			});
		DrawData = std::make_shared<RenderUsefulData>();
		TestLightData = std::make_shared<RenderUsefulData>();
		CommonShaderInput = std::make_shared<FShaderInfo>();
		ShadowShaderInput = std::make_shared<FShaderInfo>();
	}


	FPCRenderer::~FPCRenderer()
	{

	}

	bool FPCRenderer::Initialize()
	{
		if (!RHIIns->InitRenderPlatform(CharalotteEngine::GetInstance().GetWindowPtr()))
		{
			return false;
		}
		RHIIns->OnResize();
		BuildCommonInputLayout();
		BuildShadowInputLayout();
		RHIIns->BuildRootSignature(Default);
		RHIIns->BuildPSO();
		RHIIns->BuildRootSignature(Shadow);
		RHIIns->BuildShadowPSO();
		RHIIns->InitShadowMap();
		RHIIns->FlushCommandQueue();
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
		RHIIns->OpenCommandList(true);
		UpdateShadowPassCB();
		InitLight();
		FScene::GetInstance().GetCamera()->GetCameraData(DrawData->MainCameraData);
		FScene::GetInstance().GetCamera()->GetVPTransform(DrawData->VPTransform.VPMatrix);
		const auto& Actors = FScene::GetInstance().GetActorInfos();
		const auto& ActorIter = Actors.find(NowMapName);


		if (ActorIter != Actors.end())
		{
			// draw shadow
			RHIIns->DrawPrepare(Shadow);
			for (const auto& ActorPri : ActorIter->second.ActorsInfo)
			{
				ObjectConstants objConstants;
				UpdateShadowCons(objConstants, ActorPri);
				RHIIns->SetPipelineParamter(Shadow, ActorPri,
					TestLightData.get(), CharalotteEngine::GetInstance().GetRenderScenePtr());
				RHIIns->DrawMesh(ActorPri, TestLightData.get(), objConstants, CharalotteEngine::GetInstance().GetRenderScenePtr());
			}
			//RHIIns->DrawEnd(Shadow);
			RHIIns->ExecuteAndCloseCommandList();
			RHIIns->FlushCommandQueue();

			RHIIns->OpenCommandList(true);
			// draw actor
			RHIIns->DrawPrepare(Default);
			for (const auto& ActorPri : ActorIter->second.ActorsInfo)
			{
				ObjectConstants objConstants;
				UpDateCommonCons(objConstants, ActorPri);
				RHIIns->SetPipelineParamter(Default, ActorPri,
					DrawData.get(), CharalotteEngine::GetInstance().GetRenderScenePtr());
				RHIIns->DrawMesh(ActorPri, DrawData.get(), objConstants, CharalotteEngine::GetInstance().GetRenderScenePtr());
			}
			RHIIns->DrawEnd(Default);
			RHIIns->ExecuteAndCloseCommandList();
			RHIIns->SwapChain();
			RHIIns->FlushCommandQueue();
		}
	}

	bool FPCRenderer::GetIsDevicedSucceed()
	{
		return RHIIns->GetIsDeviceSucceed();
	}

	void FPCRenderer::LoadingMapDataFromAssetSystem(const std::string& MapName)
	{
		NowMapName = MapName;
		const auto& ActorPrimitiveIter = FScene::GetInstance().GetActorInfos().find(MapName);
		auto Actors = FScene::GetInstance().GetActorInfos();
		if (ActorPrimitiveIter != FScene::GetInstance().GetActorInfos().end())
		{

			RHIIns->BuildMeshAndActorPrimitives(ActorPrimitiveIter->second, FMeshAsset::GetInstance().GetMeshInfors(), CharalotteEngine::GetInstance().GetRenderScenePtr());

		}
		else
		{
			return;
		}
		// load texture
		for (const auto& TextureName : CharalotteEngine::GetInstance().GetTextureArray())
		{
			std::string TexturePath = "Content/Textures/" + TextureName + ".dds";
			RHIIns->LoadTextureResource(TextureName, TexturePath, CharalotteEngine::GetInstance().GetRenderScenePtr());
		}

		// Build scene and compile material
		RHIIns->BuildSceneResourcesForRenderPlatform(CharalotteEngine::GetInstance().GetRenderScenePtr());
		RHIIns->CompileMaterial(CharalotteEngine::GetInstance().GetRenderScenePtr());
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
		RHIIns->SetShader(CommonShaderInput);
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
		RHIIns->SetShader(ShadowShaderInput);
	}

	// re construct function
	void FPCRenderer::SceneRender()
	{	
		RHIIns->BeginFrame();
		RenderShadowDepth();
	}

	void FPCRenderer::RenderShadowDepth()
	{
		glm::mat4 LightView;
		glm::mat4 LightProj;
		BuildLightMatrix(LightView, LightProj);

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
}

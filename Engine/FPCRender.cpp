#include "stdafx.h"
#include <DirectXColors.h>
#include <cstdint>
#include <WindowsX.h>
#include <string>
#include "FPCRender.h"
#include "FDataProcessor.h"
#include "FDXResources.h"
#include "CharlotteEngine.h"
#include "FMeshAsset.h"
#include "RHIResource.h"
#include "RHIBaseData.h "
#include "DX12RHI.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace DirectX::PackedVector;
using std::string;

FPCRender::FPCRender()
{
	RHIIns = std::make_unique<DX12RHI>();
	FWinEventRegisterSystem::GetInstance().RegisterMapLoadEventForDender(Charalotte::BaseMapLoad, [this](const std::string& MapName) {
		this->LoadingMapDataFromAssetSystem(MapName);
		});
	FWinEventRegisterSystem::GetInstance().RegisterOnResize(Charalotte::DXRenderResize, [this](){
		this->RHIIns->OnResize();
	});
	DrawData = std::make_shared<Charalotte::DrawNecessaryData>();
	TestLightData = std::make_shared<Charalotte::DrawNecessaryData>();
}


FPCRender::~FPCRender()
{
	
}

bool FPCRender::Initialize()
{
	if (!RHIIns->InitRenderPlatform(CharalotteEngine::GetInstance().GetWindowPtr()))
	{
		return false;
	}
	RHIIns->OnResize();
	RHIIns->InitRenderPipeline();
	RHIIns->BuildRootSignature(Charalotte::Shadow);
	RHIIns->BuildShadowPSO();
	RHIIns->InitShadowMap();
	RHIIns->FlushCommandQueue();
	return true;

}

void FPCRender::InitLight()
{
	TestLightData->VPTransform.VPMatrix = mLightProj * mLightView;
	TestLightData->VPTransform.ProjectionTransform = mLightProj;
	TestLightData->VPTransform.ViewTransform = mLightView;
}
// draw by camera data
void FPCRender::Update()
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
		RHIIns->DrawPrepare(Charalotte::Shadow);
		for (const auto& ActorPri : ActorIter->second.ActorsInfo)
		{
			Charalotte::ObjectConstants objConstants;
			UpdateShadowCons(objConstants, ActorPri);
			RHIIns->DrawActor(ActorPri, TestLightData.get(), objConstants);
		}
		RHIIns->ExecuteAndCloseCommandList();
		RHIIns->FlushCommandQueue();

		RHIIns->OpenCommandList(true);
		// draw actor
		RHIIns->DrawPrepare(Charalotte::Default);
		for (const auto& ActorPri : ActorIter->second.ActorsInfo)
		{
			Charalotte::ObjectConstants objConstants;
			UpDateCommonCons(objConstants, ActorPri);
			RHIIns->DrawActor(ActorPri, DrawData.get(), objConstants);
		}
		RHIIns->DrawEnd();
		RHIIns->ExecuteAndCloseCommandList();
		RHIIns->SwapChain();
		RHIIns->FlushCommandQueue();
	}
}

bool FPCRender::GetIsDevicedSucceed()
{
	return RHIIns->GetIsDeviceSucceed();
}

void FPCRender::LoadingMapDataFromAssetSystem(const std::string& MapName)
{
	NowMapName = MapName;
	const auto& ActorPrimitiveIter = FScene::GetInstance().GetActorInfos().find(MapName);
	auto Actors = FScene::GetInstance().GetActorInfos();
	if (ActorPrimitiveIter != FScene::GetInstance().GetActorInfos().end())
	{

		RHIIns->BuildMeshAndActorPrimitives(ActorPrimitiveIter->second, FMeshAsset::GetInstance().GetMeshInfors());
	
	}
	else
	{
		return;
	}
	// load texture
	for (const auto& TextureName : CharalotteEngine::GetInstance().GetTextureArray())
	{
		std::string TexturePath = "Content/Textures/" + TextureName + ".dds";
		RHIIns->LoadTextureResource(TextureName, TexturePath);
	}

	// Build scene and compile material
	RHIIns->BuildSceneResourcesForRenderPlatform();
	RHIIns->CompileMaterial();
}

void FPCRender::UpdateShadowPassCB()
{
	glm::vec3 lightPos = { -5000.0f,0.0f,2500.0f };
	glm::vec3 targetPos = { 1.0f,0.0f,-0.7f };
	glm::vec3 lightUp = glm::vec3(0.0f, 0.0f, 1.0f);

	int Time = CharalotteEngine::GetInstance().GetTimer()->TotalTime();

	lightPos = glm::vec4(lightPos, 0.0f) * glm::rotate(glm::mat4(1.0f), Time % 4 * glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	targetPos = glm::vec4(targetPos, 0.0f) * glm::rotate(glm::mat4(1.0f), Time % 4 * glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
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

void FPCRender::UpdateShadowCons(Charalotte::ObjectConstants& Obj, const Charalotte::FActorInfo& ActorPri)
{
	glm::mat4 NowWorldTrans = FMathHelper::GetWorldTransMatrix(ActorPri.Transform);
	Obj.Tans = glm::transpose(mLightProj * mLightView * NowWorldTrans);
}

void FPCRender::UpDateCommonCons(Charalotte::ObjectConstants& objConstants, const Charalotte::FActorInfo& ActorPri)
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
	objConstants.Offset = CharalotteEngine::GetInstance().GetTimer()->TotalTime();
}
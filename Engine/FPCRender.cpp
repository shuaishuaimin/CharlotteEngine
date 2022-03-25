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
	RHIIns->BuildShadowPSO();
	RHIIns->BuildShadowDescriptors();
	FScene::GetInstance().GetCamera()->GetCameraData(TestLightData->MainCameraData);
	FScene::GetInstance().GetCamera()->GetVPTransform(TestLightData->VPTransform.VPMatrix);
	return true;

}

// draw by camera data
void FPCRender::Update()
{
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
			RHIIns->DrawActor(ActorPri, TestLightData.get());
		}
		RHIIns->DrawShadowEnd();

		// draw actor
		RHIIns->DrawPrepare(Charalotte::Default);
		for (const auto& ActorPri : ActorIter->second.ActorsInfo)
		{
			RHIIns->DrawActor(ActorPri, DrawData.get());
		}
		RHIIns->DrawEnd();
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


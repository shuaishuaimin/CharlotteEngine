#include "stdafx.h"
#include <DirectXColors.h>
#include <cstdint>
#include <WindowsX.h>
#include <string>
#include "FWinRender.h"
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

FWinRender::FWinRender()
{
	RHIIns = std::make_unique<DX12RHI>();
	FWinEventRegisterSystem::GetInstance().RegisterMapLoadEventForDender(Charalotte::BaseMapLoad, [this](const std::string& MapName) {
		this->LoadingMapDataFromAssetSystem(MapName);
		});
	FWinEventRegisterSystem::GetInstance().RegisterOnResize(Charalotte::DXRenderResize, [this](){
		this->RHIIns->OnResize();
	});
	DrawData = std::make_shared<Charalotte::DrawNecessaryData>();
}


FWinRender::~FWinRender()
{
	
}

bool FWinRender::Initialize()
{
	if (!RHIIns->InitRenderPlatform(CharalotteEngine::GetInstance().GetWindowPtr()))
	{
		return false;
	}
	RHIIns->OnResize();
	RHIIns->InitRenderPipeline();
	return true;

}

// draw by camera data
void FWinRender::Update()
{
	FScene::GetInstance().GetCamera()->GetCameraData(DrawData->MainCameraData);
	FScene::GetInstance().GetCamera()->GetVPTransform(DrawData->VPTransform.VPMatrix);
	RHIIns->DrawSceneByResource(DrawData.get());
}

bool FWinRender::GetIsDevicedSucceed()
{
	return RHIIns->GetIsDeviceSucceed();
}

void FWinRender::LoadingMapDataFromAssetSystem(const std::string& MapName)
{
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

	// Build scene andl compile material
	RHIIns->BuildSceneResourcesForRenderPlatform();
	RHIIns->CompileMaterial();
}


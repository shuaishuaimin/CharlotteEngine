#include "stdafx.h"
#include <DirectXColors.h>
#include <cstdint>
#include <WindowsX.h>
#include <string>
#include "DXRender.h"
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

DXRender* DXRender::render = nullptr;

DXRender::DXRender()
{
	RHIIns = std::make_unique<DX12RHI>();
	FWinEventRegisterSystem::GetInstance().RegisterMapLoadEventForDender(Charalotte::BaseMapLoad, [this](const std::string& MapName) {
		this->LoadingMapDataFromAssetSystem(MapName);
		});
	FWinEventRegisterSystem::GetInstance().RegisterOnResize(Charalotte::DXRenderResize, [this](){
		this->RHIIns->OnResize();
	});
	assert(render == nullptr);
	render = this;
}

DXRender* DXRender::GetRender()
{
	return render;
}

DXRender::~DXRender()
{
	
}

bool DXRender::Initialize()
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
void DXRender::Update()
{
	std::shared_ptr<Charalotte::DrawNecessaryData> DrawData = std::make_shared<Charalotte::DrawNecessaryData>();
	FScene::GetInstance().GetCamera()->GetCameraData(DrawData->MainCameraData);
	FScene::GetInstance().GetCamera()->GetVPTransform(DrawData->VPTransform.VPMatrix);
	RHIIns->DrawSceneByResource(DrawData.get());
}

bool DXRender::GetIsDevicedSucceed()
{
	return RHIIns->GetIsDeviceSucceed();
}

void DXRender::LoadingMapDataFromAssetSystem(const std::string& MapName)
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
	RHIIns->BuildSceneResourceForRenderPlatform();
	RHIIns->CompileMaterial();
}


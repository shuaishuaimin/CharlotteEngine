#pragma once

#include <string>
#include <unordered_map>
#include "FWindow.h"
#include "BaseStructAllPlatform.h"
#include "FMaterial.h"
#include "RHIResource.h"
#include "Primitive.h"

class RHI
{
public:
	RHI() {}
	virtual ~RHI() {}

	virtual bool InitRenderPlatform(FWindow* WindowPtr) = 0;
	virtual bool InitRenderPipeline() = 0;
	virtual void LoadTextureResource(const std::string& FileName, const std::string& FilePath) = 0;
	virtual void OnResize() = 0;
	virtual bool GetIsDeviceSucceed() = 0;

	virtual void BuildMeshAndActorPrimitives(const Charalotte::FActorPrimitive& Actors, 
				const std::unordered_map<std::string, Charalotte::FMeshPrimitive>& Meshs) = 0;

	virtual void BuildSceneResourcesForRenderPlatform() = 0;

	virtual void CompileMaterial() = 0;

	virtual void BuildShadowPSO() = 0;

	virtual void DrawPrepare(Charalotte::PSOType PSOType) = 0;
	virtual void DrawActor(const Charalotte::FActorInfo& Actor, Charalotte::DrawNecessaryData* DrawData) = 0;
	virtual void DrawEnd() = 0;
};
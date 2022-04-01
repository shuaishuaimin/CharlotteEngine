#pragma once

#include <string>
#include <unordered_map>
#include "FWindow.h"
#include "STransformDef.h"
#include "FMaterial.h"
#include "RHIResource.h"
#include "SPrimitive.h"
#include "SEShaderElements.h"
#include "FRenderScene.h"

class RHI
{
public:
	RHI() {}
	virtual ~RHI() {}

	virtual bool InitRenderPlatform(FWindow* WindowPtr) = 0;
	virtual void LoadTextureResource(const std::string& FileName, const std::string& FilePath, FRenderScene* RenderScenePtr) = 0;
	virtual void OnResize() = 0;
	virtual bool GetIsDeviceSucceed() = 0;

	virtual void BuildMeshAndActorPrimitives(const Charalotte::FActorPrimitive& Actors, 
				const std::unordered_map<std::string, Charalotte::FMeshPrimitive>& Meshs, FRenderScene* RenderScenePtr) = 0;

	virtual void BuildSceneResourcesForRenderPlatform(FRenderScene* RenderScenePtr) = 0;

	virtual void CompileMaterial(FRenderScene* RenderScenePtr) = 0;

	virtual void BuildRootSignature(Charalotte::E_PSOTYPE psoType) = 0;
	virtual void BuildShadowPSO() = 0;
	virtual void InitShadowMap() = 0;

	virtual void DrawPrepare(Charalotte::E_PSOTYPE PSOType) = 0;
	virtual void DrawMesh(const Charalotte::FActorInfo& Actor, Charalotte::DrawNecessaryData* DrawData, 
						const Charalotte::ObjectConstants& Obj, FRenderScene* RenderScenePtr) = 0;

	virtual void DrawEnd() = 0;
	virtual void DrawShadowEnd() = 0;

	virtual void OpenCommandList(bool IsOpenPso) = 0;
	virtual void ExecuteAndCloseCommandList() = 0;
	virtual void FlushCommandQueue() = 0;

	virtual void SwapChain() = 0;
	virtual void SetShader(std::shared_ptr<Charalotte::FShaderInfo> ShaderInput) = 0;
	virtual void BuildPSO() = 0;

};
#pragma once

#include <string>
#include <unordered_map>
#include "FWindow.h"
#include "STransformDef.h"
#include "FMaterial.h"
#include "RHIResource.h"
#include "SPrimitive.h"
#include "SEShaderElements.h"
#include "FTempRenderScene.h"
#include "EHeapType.h"
#include "SEResourceElement.h"
namespace Charalotte
{
	class FRenderPSO;
	class FPCRenderTarget;
	class FRenderMesh;
	class FRenderScene;
	class FTexture;
	class FVerticesAndIndicesBuffer;
	class FPCRenderTarget;
	class FResource;
}

class RHI
{
public:
	RHI() {}
	virtual ~RHI() {}
	virtual void BuildMeshAndActorPrimitives(const Charalotte::FActorPrimitive& Actors, 
				const std::unordered_map<std::string, Charalotte::FMeshPrimitive>& Meshs, FTempRenderScene* RenderScenePtr) = 0;

	virtual void BuildSceneResourcesForRenderPlatform(FTempRenderScene* RenderScenePtr) = 0;
	virtual void BuildRootSignature(Charalotte::E_PSOTYPE psoType) = 0;
	virtual void BuildShadowPSO() = 0;
	virtual void BuildPSO() = 0;
	virtual void BeginFrame() = 0;

	virtual void CompileMaterial(FTempRenderScene* RenderScenePtr) = 0;
	virtual void CreateTextureResource(Charalotte::FTexture* Texture) = 0;
	virtual void CreateVBIBBuffer(Charalotte::FVerticesAndIndicesBuffer* VBIB) = 0;
	virtual void CreateRenderMeshResource(Charalotte::FRenderMesh* RenderMesh) = 0;
	virtual void CreateRenderMeshSrv(Charalotte::FMaterial* Mat, Charalotte::FRenderMesh* Mesh) = 0;
	virtual std::shared_ptr<Charalotte::FPCRenderTarget> CreateRenderTarget() = 0;
	virtual std::shared_ptr<Charalotte::FResource> CreateResource(Charalotte::FResourceAttributes ResourceA) = 0;

	virtual void InitShadowMap() = 0;
	virtual bool InitRenderPlatform(FWindow* WindowPtr) = 0;

	virtual void LoadTextureResource(const std::string& FileName, const std::string& FilePath, FTempRenderScene* RenderScenePtr) = 0;

	virtual void DrawPrepare(Charalotte::E_PSOTYPE PSOType) = 0;
	virtual void DrawMesh(const Charalotte::FActorInfo& Actor, Charalotte::RenderUsefulData* DrawData, 
						const Charalotte::ObjectConstants& Obj, FTempRenderScene* RenderScenePtr) = 0;
	virtual void DrawMeshFinal(Charalotte::RenderUsefulData Data, Charalotte::FRenderMesh* Mesh) = 0;
	virtual void DrawEnd(Charalotte::E_PSOTYPE PSOType) = 0;

	virtual void ExecuteAndCloseCommandList() = 0;
	virtual void EndFrame() = 0;

	virtual void FlushCommandQueue() = 0;

	virtual bool GetIsDeviceSucceed() = 0;

	virtual void OpenCommandList(bool IsOpenPso) = 0;
	virtual void OnResize() = 0;

	virtual void SwapChain() = 0;
	virtual void SetShader(std::shared_ptr<Charalotte::FShaderInfo> ShaderInput) = 0;
	virtual void SetPipelineParamter(Charalotte::E_PSOTYPE PSOType, 
				const Charalotte::FActorInfo& Actor, Charalotte::RenderUsefulData* DrawData, FTempRenderScene* RenderScenePtr) = 0;
	virtual void SetRenderTarget(Charalotte::FPCRenderTarget* RT) = 0;
	virtual void SetPSOFinal(Charalotte::FRenderPSO* Pso) = 0;
	virtual void SetHeap(Charalotte::HeapType HT) = 0;

	virtual void UpdateRenderTarget(Charalotte::FPCRenderTarget* RT, Charalotte::FResourceAttributes RA) = 0;
}; 
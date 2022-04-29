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
#include "FShadowMap.h"
namespace Charalotte
{
	class FRenderPSO;
	class FPCRenderTarget;
	class FRenderMesh;
	class FRenderScene;
	class FTexture;
	class FVBIBBuffer;
	class FPCRenderTarget;
	class FResource;
	class FShader;
	class FRenderPSO;
	class FRootForShader;

	class RHI
	{
	public:
		RHI() {}
		virtual ~RHI() {}
		virtual void BuildMeshAndActorPrimitives(const FActorPrimitive& Actors,
			const std::unordered_map<std::string, FMeshPrimitive>& Meshs, FTempRenderScene* RenderScenePtr) = 0;

		virtual void BuildSceneResourcesForRenderPlatform(FTempRenderScene* RenderScenePtr) = 0;
		virtual void BuildRootSignature(E_PSOTYPE psoType) = 0;
		virtual void BuildShadowPSO() = 0;
		virtual void BuildPSO() = 0;
		virtual void BeginFrame() = 0;

		virtual void CompileMaterial(FTempRenderScene* RenderScenePtr) = 0;
		virtual void CreateTextureResource(FTexture* Texture) = 0;
		virtual void CreateVBIBBuffer(FVBIBBuffer* VBIB) = 0;
		virtual void CreateRenderMeshResource(FRenderMesh* RenderMesh) = 0;
		virtual std::shared_ptr<FRenderMesh> CreateRenderMesh() = 0;
		virtual void CreateRenderMeshSrv(FMaterial* Mat, FRenderMesh* Mesh) = 0;
		virtual std::shared_ptr<FPCRenderTarget> CreateRenderTarget() = 0;
		virtual std::shared_ptr<FResource> CreateResource(FResourceAttributes ResourceA) = 0;
		virtual std::shared_ptr<FRenderPSO> CreatePSO(FPSOAttributes, FShader*) = 0;
		virtual std::shared_ptr<FRootForShader> CreateRoot(FShader*) = 0;
		virtual std::shared_ptr<FShadowMap> CreateShadowMap() = 0;
		virtual void ChangeResourceBarrier(FResource*, E_RESOURCE_STATE Orgin, E_RESOURCE_STATE Final, unsigned int NumBarriers) = 0;

		virtual void InitShadowMap() = 0;
		virtual bool InitRenderPlatform(FWindow* WindowPtr) = 0;

		virtual void LoadTextureResource(const std::string& FileName, const std::string& FilePath, FTempRenderScene* RenderScenePtr) = 0;

		virtual void DrawPrepare(E_PSOTYPE PSOType) = 0;
		virtual void DrawMesh(const FActorInfo& Actor, RenderUsefulData* DrawData,
			const ObjectConstants& Obj, FTempRenderScene* RenderScenePtr) = 0;
		virtual void DrawMeshFinal(RenderUsefulData Data, FRenderMesh* Mesh) = 0;
		virtual void DrawEnd(E_PSOTYPE PSOType) = 0;

		virtual void ExecuteAndCloseCommandList() = 0;
		virtual void EndFrame() = 0;

		virtual void FlushCommandQueue() = 0;

		virtual bool GetIsDeviceSucceed() = 0;

		virtual void OpenCommandList(bool IsOpenPso) = 0;
		virtual void OnResize() = 0;

		virtual void SwapChain() = 0;
		virtual void SetShader(std::shared_ptr<FShaderInfo> ShaderInput) = 0;
		virtual void SetPipelineParamter(E_PSOTYPE PSOType,
			const FActorInfo& Actor, RenderUsefulData* DrawData, FTempRenderScene* RenderScenePtr) = 0;
		virtual void SetRenderTarget(FPCRenderTarget* RT) = 0;
		virtual void SetPSOFinal(FRenderPSO* Pso) = 0;
		virtual void SetHeap(HeapType HT) = 0;
		virtual void SetShaderElement(FShader*) = 0;
		virtual void SetGraphicsRoot32BitConstants(unsigned int ParamIndex, unsigned int NumOfByteValue, void* SrcData, unsigned int Offset32BitValue) = 0;
		virtual void SetShadowMapForRT(FShadowMap*) = 0;
		virtual void SetGraphicsRootDescriptorTable(unsigned int index, HeapType, int Offest, FRenderMesh*) = 0;
		virtual void SetGraphicsRootConstantBufferView(unsigned int Index, FResource*, FRenderMesh*) = 0;
		virtual void SetCurrentBufferRenderTarget() = 0;
		virtual void SerRenderTargetOfShadowMap(FShadowMap*) = 0;

		virtual void UpdateRenderTarget(FPCRenderTarget* RT, FResourceAttributes RA) = 0;
	};
}


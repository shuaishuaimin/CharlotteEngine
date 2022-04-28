#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <windows.h>
#include <wrl.h>
#include <memory>
#include <codecvt>
#include "d3dx12.h"
#include "RHI.h"
#include "WinMaterialData.h"
#include "FUtil.h"
#include "FGameTimer.h"
#include "FMathHelper.h"
#include "FUploadBuffer.h"
#include "FWinEventRegisterSystem.h"
#include "FWinRenderScene.h"
#include "DX12RHIData.h"
#include "FDXPSOs.h"
#include "FDXShadowMap.h"
#include "FHeapManager.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Charalotte
{
	class DX12RHI : public RHI
	{
	public:
		DX12RHI();
		virtual ~DX12RHI();

		virtual void BuildMeshAndActorPrimitives(const FActorPrimitive& Actors,
			const std::unordered_map<std::string, FMeshPrimitive>& Meshs, FTempRenderScene* RenderScenePtr) override;

		virtual void BuildSceneResourcesForRenderPlatform(FTempRenderScene* RenderScenePtr) override;
		virtual void BuildRootSignature(E_PSOTYPE psoType) override;
		virtual void BuildShadowPSO() override;
		virtual void BuildPSO() override;
		virtual void BeginFrame() override;

		virtual void CompileMaterial(FTempRenderScene* RenderScenePtr) override;
		virtual void CreateTextureResource(FTexture* Texture) override;
		virtual void CreateVBIBBuffer(FVBIBBuffer* VBIB) override;
		virtual void CreateRenderMeshResource(FRenderMesh* RenderMesh) override;
		virtual void CreateRenderMeshSrv(FMaterial* Mat, FRenderMesh* Mesh) override;
		virtual std::shared_ptr<FPCRenderTarget> CreateRenderTarget() override;
		virtual std::shared_ptr<FResource> CreateResource(FResourceAttributes ResourceA) override;
		virtual std::shared_ptr<FRenderPSO> CreatePSO(FPSOAttributes, FShader*) override;
		virtual std::shared_ptr<FRootForShader> CreateRoot(FShader*) override;
		virtual std::shared_ptr<FShadowMap> CreateShadowMap() override;
		virtual std::shared_ptr<FRenderMesh> CreateRenderMesh() override;
		virtual void ChangeResourceBarrier(FResource*, E_RESOURCE_STATE Orgin, E_RESOURCE_STATE Final, unsigned int NumBarriers) override;

		virtual void InitShadowMap() override;
		virtual bool InitRenderPlatform(FWindow* WindowPtr) override;

		virtual void LoadTextureResource(const std::string& FileName, const std::string& FilePath, FTempRenderScene* RenderScenePtr) override;

		virtual void DrawPrepare(E_PSOTYPE PSOType) override;
		virtual void DrawMesh(const FActorInfo& Actor, RenderUsefulData* DrawData,
			const ObjectConstants& Obj, FTempRenderScene* RenderScenePtr) override;
		virtual void DrawEnd(E_PSOTYPE PSOType) override;
		virtual void DrawMeshFinal(RenderUsefulData Data, FRenderMesh* Mesh) override;

		virtual void ExecuteAndCloseCommandList() override;
		virtual void EndFrame() override;

		virtual void FlushCommandQueue() override;

		virtual bool GetIsDeviceSucceed() override;

		virtual void OpenCommandList(bool IsOpenPso) override;
		virtual void OnResize() override;

		virtual void SwapChain() override;
		virtual void SetShader(std::shared_ptr<FShaderInfo> ShaderInput) override;
		virtual void SetPipelineParamter(E_PSOTYPE PSOType,
			const FActorInfo& Actor, RenderUsefulData* DrawData, FTempRenderScene* RenderScenePtr) override;
		virtual void SetRenderTarget(FPCRenderTarget* RT) override;
		virtual void SetPSOFinal(FRenderPSO* Pso) override;
		virtual void SetHeap(HeapType HT) override;
		virtual void SetRenderMeshHeap(FRenderMesh*) override;
		virtual void SetCurrentBufferHeap() override;
		virtual void SetShadowMapHeap(FShadowMap*) override;
		virtual void SetShaderElement(FShader*) override;
		virtual void SetGraphicsRoot32BitConstants(unsigned int ParamIndex,
			unsigned int NumOfByteValue, void* SrcData, unsigned int Offset32BitValue)override;
		virtual void SetShadowMapForRT(FShadowMap*)override;
		virtual void SetGraphicsRootDescriptorTable(unsigned int index, HeapType, int Offest, FRenderMesh*)override;
		virtual void SetGraphicsRootConstantBufferView(unsigned int Index, FResource*, FRenderMesh*)override;
		virtual void SetCurrentBufferRenderTarget()override;
		virtual void SerRenderTargetOfShadowMap(FShadowMap*)override;

		virtual void UpdateRenderTarget(FPCRenderTarget* RT, FResourceAttributes RA) override;

	protected:
		// flush fence
		// function for render init
		void CreateCommandObjects();

		void CreateSwapChain(FWindow* WindowPtr);
		void CreateRtvAndDsvDescriptorHeaps();

		ID3D12Resource* CurrentBackBuffer()const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
		// log
		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
		// pipeline
		void BuildDXMeshPrimitives(const FActorPrimitive& ActorPrimitive,
			const std::unordered_map<std::string, FMeshPrimitive>& Meshs, FWinRenderScene* DXRenderScene);

		void CompleteDXMeshPrimitives(FWinRenderScene* DXRenderScene);
		void CalcVerticesAndIndices(const std::string& GeometryName, const FTransform& Transform,
			const std::unordered_map<std::string, FMeshPrimitive>& Meshs, FWinRenderScene* DXRenderScene);

		void BuildDXActorPrimitives(const FActorPrimitive& ActorPrimitive, FWinRenderScene* DXRenderScene);

		// Build heaps
		void BuildDescriptorHeapsAndTables(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& Heap);

		void BulidConstantBuffers(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap,
			std::shared_ptr<UploadBuffer<ObjectConstants>>& ObjectCb);

		void BulidSRV(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& SrvHeap,
			FMaterial* Material, FWinRenderScene* DXRenderScene);

		void DebugDevice();

		void RegisterPSOFunc();


	protected:
		// function for all
		inline std::string wString2String(const std::wstring& ws)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.to_bytes(ws);
		}
		inline std::wstring String2wString(const std::string& s)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.from_bytes(s);
		}
		// parameters for render
	private:
		// device
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
		// Derived class should set these in derived constructor to customize starting values
		D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		bool IsDeviceSucceed;

		// command 
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
		// Init
		Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
		static const int SwapChainBufferCount = 2;

		//resource information, include three buffer information and z-buffer
		Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

		//array to store useful information
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;


		// viewport is that the window to show us all ,
		// Scissor is that the window is to be cut and showed to us;
		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

		// Fence
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		UINT mCurrentFence = 0;

		// pipeline
		std::unique_ptr<FDXPSOs> PSOs;

		// set true to use 4X MSAA ,the default is false
		bool m4xMsaaState = false; // 4X MSAA enabled;
		UINT m4xMsaaQuality = 0;  // quality level of 4X MSAA;

		// Size
		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;
		int mClientWidth;
		int mClientHeight;
		int mCurrBackBuffer = 0;
		// Name
		std::wstring mMainWndCaption = L"Charlotte Engine"; // name

	// parameters for self
	private:
		std::unordered_map<E_PSOTYPE, std::function<void()>> PsoPrepareFunction;
		std::unordered_map<E_PSOTYPE, std::function<void()>> PsoEndFunctions;
		std::unordered_map<E_PSOTYPE, std::function<void(const FActorInfo& Actor,
			RenderUsefulData* DrawData, FTempRenderScene* RenderScenePtr)>> PsoSetParamterFunctions;
		std::unique_ptr<FDXShadowMap> ShadowMap;
		std::shared_ptr<FDevice> DevicePtr;
		std::unique_ptr<FHeapManager> HeapMgr;
		std::vector<std::shared_ptr<FPCRenderTarget>> RTs;
	};


}
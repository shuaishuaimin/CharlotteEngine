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

class DX12RHI : public RHI
{
public:
	DX12RHI();
	virtual ~DX12RHI(); 

	virtual void BuildMeshAndActorPrimitives(const Charalotte::FActorPrimitive& Actors,
		const std::unordered_map<std::string, Charalotte::FMeshPrimitive>& Meshs, FTempRenderScene* RenderScenePtr) override;

	virtual void BuildSceneResourcesForRenderPlatform(FTempRenderScene* RenderScenePtr) override;
	virtual void BuildRootSignature(Charalotte::E_PSOTYPE psoType) override;
	virtual void BuildShadowPSO() override;
	virtual void BuildPSO() override;
	virtual void BeginFrame() override;

	virtual void CompileMaterial(FTempRenderScene* RenderScenePtr) override;
	virtual void CreateTextureResource(Charalotte::FTexture* Texture) override;
	virtual void CreateVBIBBuffer(Charalotte::FVerticesAndIndicesBuffer* VBIB) override;
	virtual void CreateRenderMeshResource(Charalotte::FRenderMesh* RenderMesh) override;
	virtual void CreateRenderMeshSrv(Charalotte::FMaterial* Mat, Charalotte::FRenderMesh* Mesh) override;

	virtual void InitShadowMap() override;
	virtual bool InitRenderPlatform(FWindow* WindowPtr) override;

	virtual void LoadTextureResource(const std::string& FileName, const std::string& FilePath, FTempRenderScene* RenderScenePtr) override;

	virtual void DrawPrepare(Charalotte::E_PSOTYPE PSOType) override;
	virtual void DrawMesh(const Charalotte::FActorInfo& Actor, Charalotte::RenderUsefulData* DrawData,
		const Charalotte::ObjectConstants& Obj, FTempRenderScene* RenderScenePtr) override;
	virtual void DrawEnd(Charalotte::E_PSOTYPE PSOType) override;
	virtual void DrawMeshFinal(Charalotte::RenderUsefulData Data, Charalotte::FRenderMesh* Mesh) override;

	virtual void ExecuteAndCloseCommandList() override;
	virtual void EndFrame() override;

	virtual void FlushCommandQueue() override;

	virtual bool GetIsDeviceSucceed() override;

	virtual void OpenCommandList(bool IsOpenPso) override;
	virtual void OnResize() override;

	virtual void SwapChain() override;
	virtual void SetShader(std::shared_ptr<Charalotte::FShaderInfo> ShaderInput) override;
	virtual void SetPipelineParamter(Charalotte::E_PSOTYPE PSOType,
		const Charalotte::FActorInfo& Actor, Charalotte::RenderUsefulData* DrawData, FTempRenderScene* RenderScenePtr) override;
	virtual void SetRenderTarget(Charalotte::FRenderTarget* RT) override;
	virtual void SetPSOFinal(Charalotte::FRenderPSO* Pso) override;
	virtual void SetHeap(Charalotte::HeapType HT) override;

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
	void BuildDXMeshPrimitives(const Charalotte::FActorPrimitive& ActorPrimitive, 
			const std::unordered_map<std::string, Charalotte::FMeshPrimitive>& Meshs, FWinRenderScene* DXRenderScene);

	void CompleteDXMeshPrimitives(FWinRenderScene* DXRenderScene);
	void CalcVerticesAndIndices(const std::string& GeometryName, const Charalotte::FTransform& Transform,
			const std::unordered_map<std::string, Charalotte::FMeshPrimitive>& Meshs, FWinRenderScene* DXRenderScene);
	
	void BuildDXActorPrimitives(const Charalotte::FActorPrimitive& ActorPrimitive, FWinRenderScene* DXRenderScene);

	// Build heaps
	void BuildDescriptorHeapsAndTables(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& Heap);

	void BulidConstantBuffers(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap,
		std::shared_ptr<UploadBuffer<Charalotte::ObjectConstants>>& ObjectCb);

	void BulidSRV(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& SrvHeap, 
		Charalotte::FMaterial* Material, FWinRenderScene* DXRenderScene);

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
	std::unique_ptr<Charalotte::FDXPSOs> PSOs; 

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
	std::unordered_map<Charalotte::E_PSOTYPE, std::function<void()>> PsoPrepareFunction;
	std::unordered_map<Charalotte::E_PSOTYPE, std::function<void()>> PsoEndFunctions;
	std::unordered_map<Charalotte::E_PSOTYPE, std::function<void(const Charalotte::FActorInfo& Actor,
		Charalotte::RenderUsefulData* DrawData, FTempRenderScene* RenderScenePtr)>> PsoSetParamterFunctions;
	std::unique_ptr<FDXShadowMap> ShadowMap;
	std::shared_ptr<FDevice> DevicePtr;
	std::unique_ptr<Charalotte::FHeapManager> HeapMgr;
};


#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "FUtil.h"
#include "FRender.h"
#include "FGameTimer.h"
#include "FWin32Window.h"
#include "FMathHelper.h"
#include "FUploadBuffer.h"
#include "WinBaseData.h"
#include "FCamera.h"
#include "FWinEventRegisterSystem.h"
#include "FScene.h"

// link necessary lib
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class DXRender : public FRender
{
public:
	DXRender();
	// ban copy constructor and assignment
	DXRender(const DXRender& dm) = delete;
	DXRender operator= (const DXRender& dm) = delete;
	virtual ~DXRender();

	virtual bool Initialize()override;

	void LoadingMapDataFromAssetSystem(const std::string& MapName);

	void Destory(){};

	static DXRender* GetRender();

	HINSTANCE Inst()const;

	//************************************
	// Method:    Get4xMsaaState
	// FullName:  D3DApp::Get4xMsaaState
	// Access:    public 
	// Returns:   bool
	// Qualifier: const
	//************************************
	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);

	virtual void Update() override;

	virtual bool GetIsDevicedSucceed()override;

protected:
	void OnResize();
	void Draw();
	
protected:
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();
	void FlushCommandQueue();
	void CreateRtvAndDsvDescriptorHeaps();

	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	// calculate the frame state
	void CalculateFrameStats();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
protected:

	// render pipeline
	void BuildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap);
	void BulidConstantBuffers(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap,
		std::shared_ptr<UploadBuffer<Charalotte::ObjectConstants>>& ObjectCb);

	void BuildRootSignature();
	void BuildShadersAndInputLayOut();
	void BuildMeshGeometrys();

	void BuildPSO();

private:

	static DXRender* render;

	HINSTANCE mhAppInst = nullptr; // application instance handle
	bool	  mFullscreenState = false; // full screen enabled

	// set true to use 4X MSAA ,the default is false
	bool m4xMsaaState = false; // 4X MSAA enabled;
	UINT m4xMsaaQuality = 0;  // quality level of 4X MSAA;

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	UINT mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
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

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	// Derived class should set these in derived constructor to customize starting values
	std::wstring mMainWndCaption = L"Charlotte Engine"; // name
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidth = 1980;
	int mClientHeight = 1280;

	std::vector<std::shared_ptr<Charalotte::FActorAsset>> ActorArray;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	POINT mLastMousePos;

	std::string NowMapName;

	bool IsDeviceSucceed;
};
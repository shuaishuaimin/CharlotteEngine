#pragma once


#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "FUtil.h"
#include "FApp.h"
#include "FGameTimer.h"
#include "FWin32Window.h"

// link necessary lib
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class FWinsApp : public FApp
{
protected:
	//ban  struct
	FWinsApp(HINSTANCE hInstance);
	FWinsApp(const FWinsApp& rhs) = delete;
	FWinsApp& operator=(const FWinsApp& rhs) = delete;
	virtual ~FWinsApp();

public:

	static FWinsApp* GetApp();

	HINSTANCE AppInst()const;
	//HWND MainWnd()const;
	float AspectRatio()const;

	//************************************
	// Method:    Get4xMsaaState
	// FullName:  D3DApp::Get4xMsaaState
	// Access:    public 
	// Returns:   bool
	// Qualifier: const
	//************************************
	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);

	virtual int Run() override;

	virtual bool Initialize()override;

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual std::shared_ptr<FWindow> CreateMainWindow()override;
	virtual FApp* GetOwnApp()override;

protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();
	virtual void OnResize();
	virtual void Update(const FGameTimer& gt) = 0;
	virtual void Draw(const FGameTimer& gt) = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}

protected:

	bool InitMainWindow();
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();

	void FlushCommandQueue();

	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	// calculate the frame state
	void CalculateFrameStats();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

protected:

	static FWinsApp* mApp;

	std::shared_ptr<FWin32Window> mWindowIns;

	HINSTANCE mhAppInst = nullptr; // application instance handle
	HWND	  mhMainWnd = nullptr; // main window handle
	bool	  mAppPaused = false; // is the application paused?
	bool	  mMinimized = false; // is the application min?
	bool	  mMaximized = false; // is the application max?
	bool	  mResizing = false; // are the resize bars being dragged?
	bool	  mFullscreenState = false; // full screen enabled

	// set true to use 4X MSAA ,the default is false
	bool m4xMsaaState = false; // 4X MSAA enabled;
	UINT m4xMsaaQuality = 0;  // quality level of 4X MSAA;

	// used to keep track of the delta-time? and game time (?.4).
	FGameTimer mTimer;

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
};
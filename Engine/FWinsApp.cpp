#include "stdafx.h"
#include "FWinsApp.h"
#include "FScene.h"
#include <WindowsX.h>

using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

FWinsApp* FWinsApp::mApp = nullptr;
FWinsApp* FWinsApp::GetApp()
{
	return mApp;
}

FWin32Window* FWinsApp::GetWindow()
{
	return mWindowIns.get();
}

FWinsApp::FWinsApp(HINSTANCE hInstance) : mhAppInst(hInstance)
{
	// only one D3DApp can be construct
	assert(mApp == nullptr);
	mApp = this;
}

FWinsApp::~FWinsApp()
{
	// if device is empty, flush the command list(queue)
	if (md3dDevice != nullptr)
	{
		FlushCommandQueue();
		FScene::GetInstance().SetIsDeviceSucceed(false);
	}
	FScene::GetInstance().SetIsDeviceSucceed(false);
}

HINSTANCE FWinsApp::AppInst()const
{
	return mhAppInst;
}

float FWinsApp::AspectRatio()const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

bool FWinsApp::Get4xMsaaState()const
{
	return m4xMsaaState;
}

void FWinsApp::Set4xMsaaState(bool value)
{
	if (m4xMsaaState != value)
	{
		m4xMsaaState = value;

		// Recreate the swap chain and buffers with new multi sample settings
		CreateSwapChain();
		OnResize();
	}
}

// main thread
int FWinsApp::Run()
{
	MSG msg = { 0 };

	FScene::GetInstance().GetTimer()->Reset();

	// if message is not wm_quit. Refresh the window
	while (msg.message != WM_QUIT)
	{
		// if there are Window messages then process them
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// otherwise, do animation/game stuff
		else
		{
			FScene::GetInstance().GetTimer()->Tick();

			// if game pause sleep for wait
			// else calculate frame states and update timer, draw timer to screen
			if (!FScene::GetInstance().GetIsAppPaused())
			{
				CalculateFrameStats();
				Update();
				Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}
#if defined(DEBUG) || defined(_DEBUG)
{
	ID3D12DebugDevice* pDebugDevice = nullptr;
	md3dDevice->QueryInterface(&pDebugDevice);
	pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	pDebugDevice->Release();
}
#endif

	return (int)msg.wParam;
}

bool FWinsApp::Initialize()
{
	if (mWindowIns == nullptr)
	{
		std::stringstream ss;
		ss << "No Window";
		OutputDebugStringA(ss.str().c_str());
		return false;
	}
	// only mainwindow or directx3D is not inited, return initialize failed;

	if (!InitDirect3D()) return false;

	// init resize code
	OnResize();

	return true;
}

// when input heap, we should two heap, that is rtvHeap and dsvHeap
// their type is D3D12_DESCRIPTOR_HEAP_DESC
// we should create description and its own information, that is way to explain heap 
//	data in heap can not be explained by computer without explain
void FWinsApp::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	// create heap ,if failed, throw exception
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	// create dsvHeapDesc
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

// resize
// we must create device, swapchain and directcmdlist
void FWinsApp::OnResize()
{
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);

	// flush before changing any resources
	FlushCommandQueue();

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// release the previous resources we will be recreating.
	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		mSwapChainBuffer[i].Reset();
	}
	mDepthStencilBuffer.Reset();

	// resize the swap chain
	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		mClientWidth, mClientHeight,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrBackBuffer = 0;

	// handle ??
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}

	//create the depth stencil buffer and view
	D3D12_RESOURCE_DESC depthStenciLDesc;
	depthStenciLDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStenciLDesc.Alignment = 0;
	depthStenciLDesc.Width = mClientWidth;
	depthStenciLDesc.Height = mClientHeight;
	depthStenciLDesc.DepthOrArraySize = 1;
	depthStenciLDesc.MipLevels = 1;

	// correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from
	// the depth. Therefore, because we need to create two views to the same resources:
	// 1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	// 2. DSV format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resources with a typeless format
	depthStenciLDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStenciLDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	depthStenciLDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStenciLDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStenciLDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// clear ??
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	auto HeapPorperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&HeapPorperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStenciLDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer
	auto BinaryTransition = CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	mCommandList->ResourceBarrier(1, &BinaryTransition);

	//Execute the resize commands
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// wait until resize is complete
	FlushCommandQueue();

	// Update the viewport transform to cover the client area
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, mClientWidth, mClientHeight };
}


bool FWinsApp::InitDirect3D()
{
#if defined(DEBUG) || defined(_DEBUG)
	//enable the d3d12 debug layer
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	// Try to create hard ware device !!important
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr, D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&md3dDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&md3dDevice)));
	}

	ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&mFence)));
	if (md3dDevice)
	{
		FScene::GetInstance().SetIsDeviceSucceed(true);
	}
	else
	{
		FScene::GetInstance().SetIsDeviceSucceed(false);
	}

	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check 4x MSAA quality support for our back buffer format
	// All Direct3D 11 capable devices support 4X MSAA for all render
	// target formats, so we only need to check quality support

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(md3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels, sizeof(msQualityLevels)));

	m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");


#ifdef _DEBUG
	LogAdapters();
#endif

	// create others
	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	return true;
}

void FWinsApp::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(md3dDevice->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(), // Associated command allocator
		nullptr,				   // Initial PipelineStateObject
		IID_PPV_ARGS(mCommandList.GetAddressOf())));

	// Must close in the first time, because the first time we refer to the command list
	// this we will reset it . And it needs to be closed before reseting

	mCommandList->Close();

}

void FWinsApp::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaQuality ? (m4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = mWindowIns->MainWnd();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: swap chain uses queue to perform flush.
	ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(), &sd, mSwapChain.GetAddressOf()));
}

void FWinsApp::FlushCommandQueue()
{
	// advance the fence value to mark commands up to this fence point
	mCurrentFence++;

	// Add an instruction to the command queue to set a new fence point.
	// are on the GPU timeline, the new fence point won't be set until the GPU finished
	// processing all the commands prior to this signal()
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

	// until the GPU has finished completing commands up to this fence point
	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, L"False", false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

}
ID3D12Resource* FWinsApp::CurrentBackBuffer()const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE FWinsApp::CurrentBackBufferView()const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer, mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE FWinsApp::DepthStencilView()const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void FWinsApp::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((FScene::GetInstance().GetTimer()->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		wstring fpsstr = to_wstring(fps);
		wstring mspfstr = to_wstring(mspf);

		wstring windowText = mMainWndCaption +
			L" FPS : " + fpsstr +
			L" MSPF: " + mspfstr;

		SetWindowText(mWindowIns->MainWnd(), windowText.c_str());

		// reset
		frameCnt = 0;
		timeElapsed += 1.0f;
	}

}

void FWinsApp::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}
void FWinsApp::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, mBackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void FWinsApp::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}

std::shared_ptr<FWindow> FWinsApp::CreateMainWindow()
{
	mWindowIns = std::make_shared<FWin32Window>(mhAppInst);
	return mWindowIns;
}
FApp* FWinsApp::GetOwnApp()
{
	return this;
}

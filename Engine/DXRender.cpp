#include "stdafx.h"
#include <DirectXColors.h>
#include <cstdint>
#include <WindowsX.h>
#include <string>
#include "DXRender.h"
#include "FDataProcessor.h"
#include "FWinSceneAsset.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace DirectX::PackedVector;
using std::string;

DXRender* DXRender::render = nullptr;

DXRender::DXRender() : NowMapName("")
{
	FWinEventRegisterSystem::GetInstance().RegisterMapLoadEventForDender(Charalotte::BaseMapLoad, [this](const std::string& MapName) {
		this->LoadingMapDataFromAssetSystem(MapName);
		});
	FWinEventRegisterSystem::GetInstance().RegisterOnResize(Charalotte::DXRenderResize, [this](){
		this->OnResize();
	});
	mhAppInst = GetModuleHandle(0);
	assert(render == nullptr);
	render = this;
}

DXRender* DXRender::GetApp()
{
	return render;
}

DXRender::~DXRender()
{
	// if device is empty, flush the command list(queue)
	if (md3dDevice != nullptr)
	{
		FlushCommandQueue();
		FSceneDataManager::GetInstance().SetIsDeviceSucceed(false);
	}
	FSceneDataManager::GetInstance().SetIsDeviceSucceed(false);
}

bool DXRender::Initialize()
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

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	BuildRootSignature();
	BuildShadersAndInputLayOut();
	BuildPSO();
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	FlushCommandQueue();
	return true;
}

// main thread
void DXRender::Update()
{
	CalculateFrameStats();
	Draw();
	//#if defined(DEBUG) || defined(_DEBUG)
	//{
	//	ID3D12DebugDevice* pDebugDevice = nullptr;
	//	md3dDevice->QueryInterface(&pDebugDevice);
	//	pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	//	pDebugDevice->Release();
	//}
	//#endif

}

void DXRender::OnResize()
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

void DXRender::Draw()
{
	// Reuse the memory associated with command recording.
	// we can only  reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));

	// Set the viewport and scissor rect. This needs to be reset whenever the command list is reset.
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	auto BarrierTransPresentToRT = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mCommandList->ResourceBarrier(1, &BarrierTransPresentToRT);

	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightPink, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to
	auto CBV = CurrentBackBufferView();
	auto DSV = DepthStencilView();
	mCommandList->OMSetRenderTargets(1, &CBV, true, &DSV);

	// IA
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	for (auto& ActorIns : FSceneDataManager::GetInstance().GetSceneActorByName(NowMapName))
	{
		auto MeshGeo = ActorIns->MeshAsset;
		if (MeshGeo == nullptr)
		{
			continue;
		}
		ID3D12DescriptorHeap* descriptorHeaps[] = { ActorIns->CbvHeap.Get() };
		mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		auto VertexBufferView = MeshGeo->VertexBufferView();
		auto IndexBufferView = MeshGeo->IndexBufferView();
		mCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
		mCommandList->IASetIndexBuffer(&IndexBufferView);
		mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mCommandList->SetName(L"COOL");
		mCommandList->SetGraphicsRootDescriptorTable(0, ActorIns->CbvHeap->GetGPUDescriptorHandleForHeapStart());
		auto count = MeshGeo->DrawArgs[MeshGeo->Name].IndexCount;
		mCommandList->DrawIndexedInstanced(
			MeshGeo->DrawArgs[MeshGeo->Name].IndexCount,
			1, 0, 0, 0);
	}

	// Indicate a state transition on the resource usage.
	auto BarrierTransRTToPresent = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mCommandList->ResourceBarrier(1, &BarrierTransRTToPresent);

	// Done recording commands ! important ,otherwise gpu instantce will stop
	ThrowIfFailed(mCommandList->Close());
	
	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete, this waiting is inefficient and is done for simplicity
	// Later we will show how to organize our rendering code so we do not have to wait per frame
	FlushCommandQueue();
}

bool DXRender::InitDirect3D()
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
		FSceneDataManager::GetInstance().SetIsDeviceSucceed(true);
	}
	else
	{
		FSceneDataManager::GetInstance().SetIsDeviceSucceed(false);
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

void DXRender::BuildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&CbvHeap)));
}

void DXRender::BulidConstantBuffers(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap,
	std::shared_ptr<UploadBuffer<Charalotte::ObjectConstants>>& ObjectCb)
{
	ObjectCb = std::make_shared<UploadBuffer<Charalotte::ObjectConstants>>(md3dDevice.Get(), 1, true);

	UINT objCBByteSize = FUtil::CalcConstantBufferByteSize(sizeof(Charalotte::ObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = ObjectCb->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.

	// why we use 0??
	int boxCBufIndex = 0;

	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = FUtil::CalcConstantBufferByteSize(sizeof(Charalotte::ObjectConstants));

	md3dDevice->CreateConstantBufferView(
		&cbvDesc,
		CbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void DXRender::BuildRootSignature()
{
	// Shader programs typically require resources as input (constant buffers,
	// textures, samplers).  The root signature defines the resources the shader
	// programs expect.  If we think of the shader programs as a function, and
	// the input resources as function parameters, then the root signature can be
	// thought of as defining the function signature. 

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParamter[1];

	// Create a single descriptor table of CBVS. ??
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParamter[0].InitAsDescriptorTable(1, &cbvTable);

	// A root signature is an array of root parameters.!!!
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatures(1, slotRootParamter,
		0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single lot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializeRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatures, D3D_ROOT_SIGNATURE_VERSION_1,
		serializeRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0, serializeRootSig->GetBufferPointer(),
		serializeRootSig->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature)));
}

void DXRender::BuildShadersAndInputLayOut()
{
	HRESULT hr = S_OK;

	mvsByteCode = FUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	mpsByteCode = FUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void DXRender::BuildMeshGeometrys()
{
	for (auto& MeshGeoIter : FWinSceneAsset::GetMeshAssets())
	{
		auto MeshGeo = MeshGeoIter.second;
		const UINT vbByteSize = (UINT)MeshGeo->vertices.size() * sizeof(Charalotte::Vertex);
		const UINT ibByteSize = (UINT)MeshGeo->indices.size() * sizeof(int);
		ThrowIfFailed(D3DCreateBlob(vbByteSize, &MeshGeo->VertexBufferCPU));
		CopyMemory(MeshGeo->VertexBufferCPU->GetBufferPointer(), MeshGeo->vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &MeshGeo->IndexBufferCPU));
		CopyMemory(MeshGeo->IndexBufferCPU->GetBufferPointer(), MeshGeo->indices.data(), ibByteSize);

		MeshGeo->VertexBufferGPU = FUtil::CreateDefaultBuffer(md3dDevice.Get(),
			mCommandList.Get(), MeshGeo->vertices.data(), vbByteSize, MeshGeo->VertexBufferUploader);

		MeshGeo->IndexBufferGPU = FUtil::CreateDefaultBuffer(md3dDevice.Get(),
			mCommandList.Get(), MeshGeo->indices.data(), ibByteSize, MeshGeo->IndexBufferUploader);

		MeshGeo->VertexByteStride = sizeof(Charalotte::Vertex);
		MeshGeo->VertexBufferByteSize = vbByteSize;
		MeshGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
		MeshGeo->IndexBufferByteSize = ibByteSize;
	}
}

void DXRender::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
			mvsByteCode->GetBufferSize()
	};

	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
			mpsByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	// D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
	psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}

void DXRender::LoadingMapDataFromAssetSystem(const std::string& MapName)
{
	NowMapName = MapName;
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	BuildMeshGeometrys();
	auto& ActorDir = FSceneDataManager::GetInstance().GetActorDictionary();
	const auto& ActorIter = ActorDir.find(MapName);
	if (ActorIter != ActorDir.end())
	{
		for (auto& Actors : ActorIter->second)
		{
			BuildDescriptorHeaps(Actors->CbvHeap);
			BulidConstantBuffers(Actors->CbvHeap, Actors->ObjectCB);
		}
	}
	
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	FlushCommandQueue();
}

std::shared_ptr<FWindow> DXRender::CreateMainWindow()
{
	mWindowIns = std::make_shared<FWin32Window>(mhAppInst);
	return mWindowIns;
}

FWin32Window* DXRender::GetWindow()
{
	return mWindowIns.get();
}

HINSTANCE DXRender::AppInst()const
{
	return mhAppInst;
}

bool DXRender::Get4xMsaaState()const
{
	return m4xMsaaState;
}

void DXRender::Set4xMsaaState(bool value)
{
	if (m4xMsaaState != value)
	{
		m4xMsaaState = value;

		// Recreate the swap chain and buffers with new multi sample settings
		CreateSwapChain();
		OnResize();
	}
}

// when input heap, we should two heap, that is rtvHeap and dsvHeap
// their type is D3D12_DESCRIPTOR_HEAP_DESC
// we should create description and its own information, that is way to explain heap 
//	data in heap can not be explained by computer without explain
void DXRender::CreateRtvAndDsvDescriptorHeaps()
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

void DXRender::CreateCommandObjects()
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

void DXRender::CreateSwapChain()
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

void DXRender::FlushCommandQueue()
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

ID3D12Resource* DXRender::CurrentBackBuffer()const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DXRender::CurrentBackBufferView()const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer, mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE DXRender::DepthStencilView()const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void DXRender::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((FSceneDataManager::GetInstance().GetTimer()->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsstr = std::to_wstring(fps);
		std::wstring mspfstr = std::to_wstring(mspf);

		std::wstring windowText = mMainWndCaption +
			L" FPS : " + fpsstr +
			L" MSPF: " + mspfstr;

		SetWindowText(mWindowIns->MainWnd(), windowText.c_str());

		// reset
		frameCnt = 0;
		timeElapsed += 1.0f;
	}

}

void DXRender::LogAdapters()
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

void DXRender::LogAdapterOutputs(IDXGIAdapter* adapter)
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

void DXRender::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
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



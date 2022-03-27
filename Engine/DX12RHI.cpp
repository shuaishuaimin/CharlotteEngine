#include "stdafx.h"
#include "DX12RHI.h"
#include "FWin32Window.h"
#include "FUtil.h"
#include "RHIBaseData.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace DirectX::PackedVector;
using std::string;

DX12RHI::DX12RHI() {
	Charalotte::CameraData TempCameraData;
	//FScene::GetInstance().GetCamera()->GetCameraData(TempCameraData);
	mClientWidth = 1980;
	mClientHeight = 1280;
	IsDeviceSucceed = false;
	PSOs = std::make_unique<Charalotte::FDXPSOs>();
	RegisterPSOFunc();
}

DX12RHI::~DX12RHI()
{
	// if device is empty, flush the command list(queue)
	if (md3dDevice != nullptr)
	{
		FlushCommandQueue();
		IsDeviceSucceed = false;
	}
	IsDeviceSucceed = false;
}

void DX12RHI::LoadTextureResource(const std::string& FileName, const std::string& FilePath)
{
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	auto Texture = std::make_shared<Charalotte::DXTextureResource>();
	Texture->Name = FileName;
	Texture->Filename = String2wString(FilePath);
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Texture->Filename.c_str(),
		Texture->Resource, Texture->UploadHeap));

	FDXResources::GetInstance().AddDxTextureResource(Texture->Name, Texture);
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	FlushCommandQueue();
}

bool DX12RHI::InitRenderPlatform(FWindow* WindowPtr)
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
		IsDeviceSucceed = true;
	}
	else
	{
		IsDeviceSucceed = false;
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
	CreateSwapChain(WindowPtr);
	CreateRtvAndDsvDescriptorHeaps();
	DevicePtr = std::make_shared<FDXDevice>(md3dDevice.Get());
	return true;
}

bool DX12RHI::InitRenderPipeline()
{
	BuildShadersAndInputLayOut();
	BuildRootSignature(Charalotte::Default);
	BuildPSO();
	return true;
}

void DX12RHI::OnResize()
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

bool DX12RHI::GetIsDeviceSucceed()
{
	return IsDeviceSucceed;
}

void DX12RHI::BuildMeshAndActorPrimitives(const Charalotte::FActorPrimitive& Actors,
	const std::unordered_map<std::string, Charalotte::FMeshPrimitive>& Meshs)
{
	BuildDXMeshPrimitives(Actors, Meshs);
	BuildDXActorPrimitives(Actors);
}

void DX12RHI::BuildSceneResourcesForRenderPlatform()
{
	CompleteDXMeshPrimitives();
	for (auto& DXActorResource : FDXResources::GetInstance().GetDXActorResources())
	{
		BuildDescriptorHeapsAndTables(DXActorResource.second->CbvHeap);
		BuildDescriptorHeapsAndTables(DXActorResource.second->SrvHeap);
		BulidConstantBuffers(DXActorResource.second->CbvHeap, DXActorResource.second->ObjectCB);
	}
}

void DX12RHI::CompileMaterial()
{
	for (auto& DXActorResource : FDXResources::GetInstance().GetDXActorResources())
	{
		BulidSRV(DXActorResource.second->SrvHeap, DXActorResource.second->Material);
	}
}

void DX12RHI::DrawPrepare(Charalotte::PSOType psoType)
{
	const auto& Iter = PsoPrepareFunction.find(psoType);
	if (Iter != PsoPrepareFunction.end())
	{
		Iter->second();
	}
}
void DX12RHI::DrawActor(const Charalotte::FActorInfo& Actor, Charalotte::DrawNecessaryData* DrawData, const Charalotte::ObjectConstants& Obj)
{
	auto ActorInsPtr = FDXResources::GetInstance().GetDXActorResourcesByName(Actor.ActorPrimitiveName);
	if (ActorInsPtr == nullptr)
	{
		return;
	}
	auto MeshGeo = ActorInsPtr->DXMeshPrimitive;

	ID3D12DescriptorHeap* descriptorHeaps[] = { ActorInsPtr->SrvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	auto VertexBufferView = MeshGeo->VertexBufferView();
	auto IndexBufferView = MeshGeo->IndexBufferView();
	mCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	mCommandList->IASetIndexBuffer(&IndexBufferView);
	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mCommandList->SetName(L"COOL");

	auto count = MeshGeo->DrawArgs[MeshGeo->Name].IndexCount;
	// use resource
	
	if (!IsDrawShadow)
	{
		mCommandList->SetGraphicsRoot32BitConstants(0, 4, &(DrawData->MainCameraData.Location), 0);

		mCommandList->SetGraphicsRootConstantBufferView(1, ActorInsPtr->ObjectCB->Resource()->GetGPUVirtualAddress());

		mCommandList->SetGraphicsRootDescriptorTable(3, ActorInsPtr->SrvHeap->GetGPUDescriptorHandleForHeapStart());
		ID3D12DescriptorHeap* ShadowdescriptorHeaps[] = { ShadowMap->GetSrvHeap().Get() };
		mCommandList->SetDescriptorHeaps(_countof(ShadowdescriptorHeaps), ShadowdescriptorHeaps);
		mCommandList->SetGraphicsRootDescriptorTable(4, ShadowMap->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart());
	}
	else
	{
		mCommandList->SetGraphicsRootConstantBufferView(0, ActorInsPtr->ObjectCB->Resource()->GetGPUVirtualAddress());
	}
	// update the constant buffer with the latest worldviewproj glm::mat4
	ActorInsPtr->ObjectCB->CopyData(0, Obj);
	
	mCommandList->DrawIndexedInstanced(
		MeshGeo->DrawArgs[MeshGeo->Name].IndexCount,
		1, 0, 0, 0);
}

void DX12RHI::DrawEnd()
{
	// Indicate a state transition on the resource usage.
	auto BarrierTransRTToPresent = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mCommandList->ResourceBarrier(1, &BarrierTransRTToPresent);

	auto BarrierTransResourceToRead = CD3DX12_RESOURCE_BARRIER::Transition(ShadowMap->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	mCommandList->ResourceBarrier(1, &BarrierTransResourceToRead);
}

void DX12RHI::DrawShadowEnd()
{
	//mCommandList->SetGraphicsRootDescriptorTable(4, ShadowMap->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart());
	// Indicate a state transition on the resource usage.
	
	auto BarrierTransRTToPresent = CD3DX12_RESOURCE_BARRIER::Transition(ShadowMap->GetResource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
	mCommandList->ResourceBarrier(1, &BarrierTransRTToPresent);

	auto BarrierTransReadToResource = CD3DX12_RESOURCE_BARRIER::Transition(ShadowMap->GetResource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	mCommandList->ResourceBarrier(1, &BarrierTransReadToResource);

}

void DX12RHI::BuildShadowPSO()
{
	bool GetPSOSuccess = false;
	auto& ShadowPso = PSOs->GetPSOReference(Charalotte::Shadow, GetPSOSuccess);
	if (!GetPSOSuccess)
	{
		return;
	}
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	SecureZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.RasterizerState.DepthBias = 10000;
	psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	psoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	psoDesc.InputLayout = { ShadowPso.mInputLayout.data(), (UINT)ShadowPso.mInputLayout.size() };
	psoDesc.pRootSignature = ShadowPso.mRootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(ShadowPso.mvsByteCode->GetBufferPointer()),
			ShadowPso.mvsByteCode->GetBufferSize()
	};

	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(ShadowPso.mpsByteCode->GetBufferPointer()),
			ShadowPso.mpsByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	// D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 0;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&ShadowPso.mPSO)));
}

void DX12RHI::InitShadowMap()
{
	ShadowMap = std::make_unique<FDXShadowMap>(mClientWidth, mClientHeight, DevicePtr.get());
	ShadowMap->Init();
}
// when input heap, we should two heap, that is rtvHeap and dsvHeap
// their type is D3D12_DESCRIPTOR_HEAP_DESC
// we should create description and its own information, that is way to explain heap 
//	data in heap can not be explained by computer without explain
void DX12RHI::CalcVerticesAndIndices(const std::string& GeometryName, const Charalotte::FTransform& Transform,
	const std::unordered_map<std::string, Charalotte::FMeshPrimitive>& Meshs)
{
	if (Meshs.find(GeometryName) == Meshs.end())
	{
		return;
	}
	const Charalotte::FMeshPrimitive& MeshPri = Meshs.find(GeometryName)->second;
	
	std::string Name = GeometryName;
	if (MeshPri.LodInfos.size() <= 0)
	{
		std::stringstream ss;
		ss << GeometryName << "No result";
		OutputDebugStringA(ss.str().c_str());
		return;
	}
	std::shared_ptr<Charalotte::FDXMeshPrimitive> DXMeshPri = std::make_shared<Charalotte::FDXMeshPrimitive>();
	int VertexIndex = 0;
	// use normal to vertex color
	bool IsUseNormalToColor = false;
	if (MeshPri.LodInfos[0].VerticesLocation.size() == MeshPri.LodInfos[0].VerticesNormal.size())
	{
		IsUseNormalToColor = true;
	}

	for (const auto& VertexLocation : MeshPri.LodInfos[0].VerticesLocation)
	{
		glm::vec4 VertexColor = glm::vec4(1.0f);

		Charalotte::Vertex vertex;
		glm::vec3 Float3 = glm::vec3(1.0f);
		// execute scale transport
		Float3.x = VertexLocation.x;
		Float3.y = VertexLocation.y;
		Float3.z = VertexLocation.z;

		const auto& MeshLod = MeshPri.LodInfos[0];
		vertex.Pos = Float3;
		if (IsUseNormalToColor)
		{
			VertexColor.x = MeshLod.VerticesNormal[VertexIndex].x * 0.5f + 0.5f;
			VertexColor.y = MeshLod.VerticesNormal[VertexIndex].y * 0.5f + 0.5f;
			VertexColor.z = MeshLod.VerticesNormal[VertexIndex].z * 0.5f + 0.5f;
			VertexColor.w = MeshLod.VerticesNormal[VertexIndex].w * 0.5f + 0.5f;
		}
		vertex.Color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		vertex.Normal = VertexColor;
		vertex.UV.x = MeshLod.UVs[VertexIndex].X;
		vertex.UV.y = MeshLod.UVs[VertexIndex].Y;
		DXMeshPri->vertices.push_back(vertex);
		VertexIndex++;
	}

	for (const auto& index : MeshPri.LodInfos[0].Indices)
	{
		int32_t VertexIndex = index;
		DXMeshPri->indices.push_back(static_cast<int16_t>(VertexIndex));
	}

	Charalotte::FDXSubmeshPrimitive submesh;
	submesh.IndexCount = (UINT)(MeshPri.LodInfos[0].Indices.size());
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	DXMeshPri->DrawArgs[GeometryName] = submesh;
	DXMeshPri->Name = GeometryName;

	FDXResources::GetInstance().AddDXMeshPrimitive(GeometryName, DXMeshPri);
}

void DX12RHI::BuildDXMeshPrimitives(const Charalotte::FActorPrimitive& ActorPrimitive,
			const std::unordered_map<std::string, Charalotte::FMeshPrimitive>& Meshs)
{
	for (const auto& Actor : ActorPrimitive.ActorsInfo)
	{
		std::string MeshName = Actor.MeshPrimitiveName;
		if (MeshName.size() <= 0)
		{
			continue;
		}
		MeshName.erase(MeshName.size() - 1, 1);
		CalcVerticesAndIndices(MeshName, Actor.Transform, Meshs);
	}	
}

// build dx mesh primitives and push it into dx resource
void DX12RHI::CompleteDXMeshPrimitives()
{
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	for (auto& MeshGeoIter : FDXResources::GetInstance().GetDXMeshResources())
	{
		Charalotte::FDXMeshPrimitive* MeshGeo = MeshGeoIter.second.get();
		if (MeshGeo == nullptr)
		{
			continue;
		}
		const UINT vbByteSize = MeshGeo->GetVerticesSize();
		const UINT ibByteSize = MeshGeo->GetIndicesSize();
		ThrowIfFailed(D3DCreateBlob(vbByteSize, &MeshGeo->VertexBufferCPU));
		CopyMemory(MeshGeo->VertexBufferCPU->GetBufferPointer(), MeshGeo->vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &MeshGeo->IndexBufferCPU));
		CopyMemory(MeshGeo->IndexBufferCPU->GetBufferPointer(), MeshGeo->indices.data(), ibByteSize);

		auto Device = md3dDevice.Get();
		auto VerticeData = MeshGeo->vertices.data();
		auto Commandlist = mCommandList.Get();

		MeshGeo->VertexBufferGPU = FUtil::CreateDefaultBuffer(Device,
			Commandlist, VerticeData, vbByteSize, MeshGeo->VertexBufferUploader);

		MeshGeo->IndexBufferGPU = FUtil::CreateDefaultBuffer(md3dDevice.Get(),
			mCommandList.Get(), MeshGeo->indices.data(), ibByteSize, MeshGeo->IndexBufferUploader);

		MeshGeo->VertexByteStride = sizeof(Charalotte::Vertex);
		MeshGeo->VertexBufferByteSize = vbByteSize;
		MeshGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
		MeshGeo->IndexBufferByteSize = ibByteSize;
	}
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	FlushCommandQueue();
}

// build dx actor primitives and push it into dx resource
void DX12RHI::BuildDXActorPrimitives(const Charalotte::FActorPrimitive& ActorPrimitive)
{
	FDXResources::GetInstance().ClearDXActorPrimitives();

	const auto& ActorInfors = ActorPrimitive.ActorsInfo;
	for (const auto& Actors : ActorPrimitive.ActorsInfo)
	{
		std::string ActorMeshPrimitiveName = Actors.MeshPrimitiveName;
		if (ActorMeshPrimitiveName.size() <= 0)
		{
			continue;
		}
		ActorMeshPrimitiveName.erase(ActorMeshPrimitiveName.size() - 1, 1);
		auto MeshResourcePtr = FDXResources::GetInstance().GetDXMeshResourceByName(ActorMeshPrimitiveName);
		if (MeshResourcePtr != nullptr)
		{
			std::shared_ptr<Charalotte::FDXActorPrimitive> DXActorPrimitive = std::make_shared<Charalotte::FDXActorPrimitive>();
			DXActorPrimitive->DXMeshPrimitive = MeshResourcePtr;
			DXActorPrimitive->DXActorPrimitiveName = Actors.ActorPrimitiveName;
			DXActorPrimitive->Transform = Actors.Transform;
			DXActorPrimitive->Material = Actors.Material.get();
			FDXResources::GetInstance().AddDXActorPrimitive(Actors.ActorPrimitiveName, DXActorPrimitive);
		}
	}
}

void DX12RHI::CreateRtvAndDsvDescriptorHeaps()
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
	dsvHeapDesc.NumDescriptors = 3;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void DX12RHI::CreateCommandObjects()
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

void DX12RHI::CreateSwapChain(FWindow* WindowPtr)
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
	sd.OutputWindow = dynamic_cast<FWin32Window*>(WindowPtr)->MainWnd();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: swap chain uses queue to perform flush.
	ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(), &sd, mSwapChain.GetAddressOf()));
}

void DX12RHI::FlushCommandQueue()
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
void DX12RHI::BuildRootSignature(Charalotte::PSOType psoType)
{
	bool GetPSOSuccess = false;
	auto& Pso = PSOs->GetPSOReference(psoType, GetPSOSuccess);
	if (!GetPSOSuccess)
	{
		return;
	}
	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		Pso.mvsByteCode->GetBufferPointer(),
		Pso.mvsByteCode->GetBufferSize(),
		IID_PPV_ARGS(&Pso.mRootSignature)));

}
void DX12RHI::BuildShadersAndInputLayOut()
{
	HRESULT hr = S_OK;
	bool GetPSOSuccess = false;
	auto& Pso = PSOs->GetPSOReference(Charalotte::Default, GetPSOSuccess);
	auto& ShadowPso = PSOs->GetPSOReference(Charalotte::Shadow);
	if (!GetPSOSuccess)
	{
		return;
	}
	Pso.mvsByteCode = FUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	Pso.mpsByteCode = FUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	ShadowPso.mvsByteCode = FUtil::CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "VS", "vs_5_0");
	ShadowPso.mpsByteCode = FUtil::CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "PS", "ps_5_0");
	Pso.mInputLayout = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	ShadowPso.mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} ,
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}
void DX12RHI::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	bool GetPSOSuccess = false;
	auto& Pso = PSOs->GetPSOReference(Charalotte::Default, GetPSOSuccess);
	if (!GetPSOSuccess)
	{
		return;
	}
	SecureZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { Pso.mInputLayout.data(), (UINT)Pso.mInputLayout.size() };
	psoDesc.pRootSignature = Pso.mRootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(Pso.mvsByteCode->GetBufferPointer()),
			Pso.mvsByteCode->GetBufferSize()
	};

	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(Pso.mpsByteCode->GetBufferPointer()),
			Pso.mpsByteCode->GetBufferSize()
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
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&Pso.mPSO)));
}

// Build heaps
void DX12RHI::BuildDescriptorHeapsAndTables(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& Heap)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 10;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&Heap)));
}

void DX12RHI::BulidConstantBuffers(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap,
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

void DX12RHI::BulidSRV(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& SrvHeap, 
			FMaterial* Material)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	std::string TextureName = Material->GetTexture();
	std::string NormalName = Material->GetNormal();

	const auto& TextureResource = FDXResources::GetInstance().GetDXTextResourceByName(TextureName);
	const auto& NormalResource = FDXResources::GetInstance().GetDXTextResourceByName(NormalName);
	srvDesc.Format = TextureResource->Resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = TextureResource->Resource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = SrvHeap->GetCPUDescriptorHandleForHeapStart();

	md3dDevice->CreateShaderResourceView(
		TextureResource->Resource.Get(),
		&srvDesc,
		CPUHandle);
	CPUHandle.ptr += mCbvSrvUavDescriptorSize;
	md3dDevice->CreateShaderResourceView(
		NormalResource->Resource.Get(),
		&srvDesc,
		CPUHandle);
}
D3D12_CPU_DESCRIPTOR_HANDLE DX12RHI::DepthStencilView()const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}
ID3D12Resource* DX12RHI::CurrentBackBuffer()const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}
D3D12_CPU_DESCRIPTOR_HANDLE DX12RHI::CurrentBackBufferView()const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer, mRtvDescriptorSize);
}

void DX12RHI::LogAdapters()
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

void DX12RHI::LogAdapterOutputs(IDXGIAdapter* adapter)
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

void DX12RHI::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
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

void DX12RHI::DebugDevice()
{
#if defined(DEBUG) || defined(_DEBUG)
{
	ID3D12DebugDevice* pDebugDevice = nullptr;
	md3dDevice->QueryInterface(&pDebugDevice);
	pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	pDebugDevice->Release();
}
#endif
}

void DX12RHI::RegisterPSOFunc()
{
	PsoPrepareFunction.insert({Charalotte::Default, [this](){
		bool IsPSOGetSucceed = false;
		IsDrawShadow = false;
		Charalotte::PSO& Pso = PSOs->GetPSOReference(Charalotte::Default, IsPSOGetSucceed);
		if (!IsPSOGetSucceed)
		{
			return;
		}
		
		
		// Set the viewport and scissor rect. This needs to be reset whenever the command list is reset.
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);
		
		// Indicate a state transition on the resource usage.
		auto BarrierTransPresentToRT = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mCommandList->ResourceBarrier(1, &BarrierTransPresentToRT);

		auto BarrierWriteToResource = CD3DX12_RESOURCE_BARRIER::Transition(ShadowMap->GetResource(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		mCommandList->ResourceBarrier(1, &BarrierWriteToResource);

		// Clear the back buffer and depth buffer.
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightPink, 0, nullptr);
		// Specify the buffers we are going to render to
		auto CBV = CurrentBackBufferView();
		auto DSV = DepthStencilView();
		mCommandList->OMSetRenderTargets(1, &CBV, true, &DSV);

		// IA
		mCommandList->SetGraphicsRootSignature(Pso.mRootSignature.Get());
		mCommandList->SetPipelineState(Pso.mPSO.Get());
	}});

	PsoPrepareFunction.insert({ Charalotte::Shadow,[this](){

		Charalotte::PSO& ShadowPso = PSOs->GetPSOReference(Charalotte::Shadow);
		Charalotte::PSO& DefaultPso = PSOs->GetPSOReference(Charalotte::Default);

		if (ShadowMap == nullptr)
		{
			return;
		}
		IsDrawShadow = true;
		ID3D12DescriptorHeap* descriptorHeaps[] = { ShadowMap->GetSrvHeap().Get() };
		mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		mCommandList->SetGraphicsRootSignature(ShadowPso.mRootSignature.Get());

		mCommandList->ClearDepthStencilView(ShadowMap->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		auto DsvHeapStart = ShadowMap->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart();
		mCommandList->OMSetRenderTargets(0, nullptr, false, &DsvHeapStart);

		mCommandList->SetPipelineState(ShadowPso.mPSO.Get());

		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

	}});
}
void DX12RHI::OpenCommandList(bool IsOpenPso)
{
	ThrowIfFailed(mDirectCmdListAlloc->Reset());
	auto& Pso = PSOs->GetPSOReference(Charalotte::Default);
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), Pso.mPSO.Get()));
}
void DX12RHI::ExecuteAndCloseCommandList()
{
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* ThiscmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(ThiscmdsLists), ThiscmdsLists);
}

void DX12RHI::SwapChain()
{
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
}
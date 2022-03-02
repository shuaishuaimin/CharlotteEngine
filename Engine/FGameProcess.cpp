#include "stdafx.h"
#include <DirectXColors.h>
#include "FGameProcess.h"
#include "FDataProcessor.h"
#include "FSceneAsset.h"
#include <cstdint>


using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace DirectX::PackedVector;
using std::string;

FGameProcess::FGameProcess(HINSTANCE hInstance) : FWinsApp(hInstance), ColorIndex(0)
{
	Charalotte::CameraData CameraData;
	CameraData.Near = 1.0f;
	CameraData.Far = 7000.0f;
	CameraData.FovAngleY = 0.25f * FMathHelper::Pi;
	CameraData.AspectRatio = AspectRatio();
	CameraData.Location = XMVectorSet(-5000.0f, 0.0f, 0.0f, 1.0f);
	CameraData.Target = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	CameraData.Up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	MainCamera = std::make_unique<FCamera>(CameraData);
}
FGameProcess::~FGameProcess()
{

}

bool FGameProcess::Initialize()
{
	if (!FWinsApp::Initialize())
		return false;

	// reset

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	BuildRootSignature();
	LoadMeshs("ThirdPersonExampleMap.dat");
	LoadActors(ActorInfos);
	BuildShadersAndInputLayOut();
	BuildPSO();

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	FlushCommandQueue();
	return true;
}

void FGameProcess::OnResize()
{
	FWinsApp::OnResize();
}

void FGameProcess::Update(const FGameTimer& gt)
{
	OnKeyBoardInput(gt);
	for (auto& ActorIns : ActorArray)
	{
		// update the constant buffer with the latest worldviewproj matrix
		Charalotte::ObjectConstants objConstants;
		XMMATRIX NowVPTrans;
		MainCamera->GetVPTransform(NowVPTrans);
		XMMATRIX NowWorldTrans = FMathHelper::GetWorldTransMatrix(ActorIns->Transform);
		XMMATRIX NowMVPTrans = NowWorldTrans * NowVPTrans;
		XMStoreFloat4x4(&objConstants.TransMatrix, XMMatrixTranspose(NowMVPTrans));
		ActorIns->ObjectCB->CopyData(0, objConstants);
	}
}

void FGameProcess::Draw(const FGameTimer& gt)
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

	for (auto& ActorIns : ActorArray)
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

void FGameProcess::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}
void FGameProcess::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}
void FGameProcess::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// make each pixel correspond to a quarter of a degree
		float dx = -0.0001f * static_cast<float>(x - mLastMousePos.x);
		float dy = -0.1f * static_cast<float>(y - mLastMousePos.y);
		CameraTrans.pitch += dx;
		//CameraTrans.row += dy;
		MainCamera->TransformCamera(CameraTrans);
		CameraTrans = DefaultCameraTrans;
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene
		float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);
		CameraTrans.Translation.x += (dx + dy);
		MainCamera->TransformCamera(CameraTrans);
		CameraTrans = DefaultCameraTrans;
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
void FGameProcess::OnKeyBoardInput(const FGameTimer& gt)
{
	float Se = 0.3f;
	if (GetAsyncKeyState('A') & 0x8000)
	{
		CameraTrans.Translation.y += Se;
		MainCamera->TransformCamera(CameraTrans);
		CameraTrans = DefaultCameraTrans;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		CameraTrans.Translation.y -= Se;
		MainCamera->TransformCamera(CameraTrans);
		CameraTrans = DefaultCameraTrans;
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		CameraTrans.Translation.z -= Se;
		MainCamera->TransformCamera(CameraTrans);
		CameraTrans = DefaultCameraTrans;
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		CameraTrans.Translation.z += Se;
		MainCamera->TransformCamera(CameraTrans);
		CameraTrans = DefaultCameraTrans;
	}
}

void FGameProcess::BuildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&CbvHeap)));
}
void FGameProcess::BulidConstantBuffers(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& CbvHeap,
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

void FGameProcess::BuildRootSignature()
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
void FGameProcess::BuildShadersAndInputLayOut()
{
	HRESULT hr = S_OK;

	mvsByteCode = FUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	mpsByteCode = FUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}


void FGameProcess::CalcVerticesAndIndices(const std::string& GeometryName, const Charalotte::FTransform& Transform)
{
	Charalotte::FMeshInfoForPrint MeshInfo;
	std::shared_ptr<MeshGeometry> MeshGeo = std::make_shared<MeshGeometry>();
	// save mesh data buffer
	auto MeshInfoFind = MeshInfoDir.find(GeometryName);
	if (MeshInfoFind != MeshInfoDir.end())
	{
		MeshInfo = MeshInfoFind->second;
		return;
	}
	else
	{
		FDataProcessor::LoadMesh(GeometryName, MeshInfo);
		MeshInfoDir.insert(std::make_pair(GeometryName, MeshInfo));
	}
	
	std::string Name = GeometryName;
	if (MeshInfo.LodInfos.size() <= 0)
	{
		std::stringstream ss;
		ss << GeometryName << "No result";
		OutputDebugStringA(ss.str().c_str());
		return;
	}

	int VertexIndex = 0;
	// use normal to vertex color
	bool IsUseNormalToColor = false;
	if (MeshInfo.LodInfos[0].VerticesLocation.size() == MeshInfo.LodInfos[0].VerticesNormal.size())
	{
		IsUseNormalToColor = true;
	}

	for (const auto& VertexLocation : MeshInfo.LodInfos[0].VerticesLocation)
	{
		XMFLOAT4 VertexColor = XMFLOAT4(Colors::DarkGray);

		Charalotte::Vertex vertex;
		XMFLOAT3 Float3;
		// execute scale transport
		Float3.x = VertexLocation.x;
		Float3.y = VertexLocation.y;
		Float3.z = VertexLocation.z;

		vertex.Pos = Float3;
		if (IsUseNormalToColor)
		{
			VertexColor.x = MeshInfo.LodInfos[0].VerticesNormal[VertexIndex].x * 0.5f + 0.5f;
			VertexColor.y = MeshInfo.LodInfos[0].VerticesNormal[VertexIndex].y * 0.5f + 0.5f;
			VertexColor.z = MeshInfo.LodInfos[0].VerticesNormal[VertexIndex].z * 0.5f + 0.5f;
			VertexColor.w = MeshInfo.LodInfos[0].VerticesNormal[VertexIndex].w * 0.5f + 0.5f;
		}
		vertex.Color = VertexColor;
		MeshGeo->vertices.push_back(vertex);
		VertexIndex ++;
	}

	for (const auto& index : MeshInfo.LodInfos[0].Indices)
	{
		int32_t VertexIndex = index;
		MeshGeo->indices.push_back(static_cast<int16_t>(VertexIndex));
	}

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)(MeshInfo.LodInfos[0].Indices.size());
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	MeshGeo->DrawArgs[GeometryName] = submesh;
	MeshGeo->Name = GeometryName;

	FSceneAsset::AddMeshData(GeometryName, MeshGeo);

	//MeshGeoArray.push_back(MeshGeo);
}

void FGameProcess::BuildMeshGeometrys()
{
	for (auto& MeshGeoIter : FSceneAsset::GetMeshAssets())
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
void FGameProcess::BuildEnviroument(const std::string& GeometryName)
{
	Charalotte::FActorsInfoForPrint ActorInfos;
	FDataProcessor::LoadActors(GeometryName, ActorInfos);
	if (ActorInfos.ActorsInfo.size() <= 0) return;
	for (const auto& EnviroumentActor : ActorInfos.ActorsInfo)
	{
		std::string assetName = EnviroumentActor.AssetName;
		if (assetName.size() <= 0)
		{
			continue;
		}
		assetName.erase(assetName.size() - 1, 1);
		assetName += ".dat";
		//OutputDebugStringA(assetName.c_str());
		CalcVerticesAndIndices(assetName, EnviroumentActor.Transform);
	}
	BuildMeshGeometrys();
}

void FGameProcess::BuildPSO()
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

void FGameProcess::LoadMeshs(const std::string& GeometryName)
{
	FDataProcessor::LoadActors(GeometryName, ActorInfos);
	if (ActorInfos.ActorsInfo.size() <= 0) return;
	for (const auto& EnviroumentActor : ActorInfos.ActorsInfo)
	{
		std::string assetName = EnviroumentActor.AssetName;
		if (assetName.size() <= 0)
		{
			continue;
		}
		assetName.erase(assetName.size() - 1, 1);
		assetName += ".dat";
		//OutputDebugStringA(assetName.c_str());
		CalcVerticesAndIndices(assetName, EnviroumentActor.Transform);
	}
	BuildMeshGeometrys();
}

void FGameProcess::LoadActors(const Charalotte::FActorsInfoForPrint& ActorInfoIn)
{
	if (ActorInfoIn.ActorsInfo.size() <= 0) return;
	for (const auto& EnviroumentActor : ActorInfoIn.ActorsInfo)
	{
		std::string assetName = EnviroumentActor.AssetName;
		if (assetName.size() <= 0)
		{
			continue;
		}
		assetName.erase(assetName.size() - 1, 1);
		assetName += ".dat";
		std::shared_ptr<FActorAsset> ActorAsset = std::make_shared<FActorAsset>();
		ActorAsset->MeshAsset = FSceneAsset::GetMeshAsset(assetName);
		ActorAsset->Transform = EnviroumentActor.Transform;
	
		BuildDescriptorHeaps(ActorAsset->CbvHeap);
		BulidConstantBuffers(ActorAsset->CbvHeap, ActorAsset->ObjectCB);

		ActorArray.push_back(ActorAsset);
	}
}
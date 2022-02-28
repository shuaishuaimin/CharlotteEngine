#include "stdafx.h"
#include <DirectXColors.h>
#include "FGameProcess.h"
#include "FDataProcessor.h"
#include <cstdint>


using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace DirectX::PackedVector;
using std::string;

FGameProcess::FGameProcess(HINSTANCE hInstance) : FWinsApp(hInstance), ColorIndex(0), VertexNumDrawed(0)
{
	TestColors.push_back(XMFLOAT4(Colors::Blue));
	TestColors.push_back(XMFLOAT4(Colors::DarkGray));
	TestColors.push_back(XMFLOAT4(Colors::DarkCyan));
	TestColors.push_back(XMFLOAT4(Colors::Green));
	TestColors.push_back(XMFLOAT4(Colors::Yellow));
	TestColors.push_back(XMFLOAT4(Colors::Aqua));
	TestColors.push_back(XMFLOAT4(Colors::Black));
	TestColors.push_back(XMFLOAT4(Colors::White));
	TestColors.push_back(XMFLOAT4(Colors::DarkMagenta));
	TestColors.push_back(XMFLOAT4(Colors::Bisque));
	TestColors.push_back(XMFLOAT4(Colors::BlanchedAlmond));
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
	BuildDescriptorHeaps();
	BulidConstantBuffers();
	BuildRootSignature();
	BuildShadersAndInputLayOut();
	//BuildBoxGeometry("MatineeCam_SM.dat");
	BuildEnviroument("ThirdPersonExampleMap.dat");
	BuildPSO();
	TestFunc();

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	FlushCommandQueue();
	return true;
}

void FGameProcess::OnResize()
{
	FWinsApp::OnResize();
	ConstructProjectionMatrix();
}

void FGameProcess::ConstructProjectionMatrix()
{
	// The windows resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * FMathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void FGameProcess::Update(const FGameTimer& gt)
{
	// Convert spherical to cartesian coordinates
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	// build the view matrix
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world * view * proj;

	// update the constant buffer with the latest worldviewproj matrix
	Charalotte::ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.TransMatrix, XMMatrixTranspose(worldViewProj));
	mObjectCB->CopyData(0, objConstants);
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

	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// IA
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	auto VertexBufferView = mMeshGeo->VertexBufferView();
	auto IndexBufferView = mMeshGeo->IndexBufferView();
	mCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	mCommandList->IASetIndexBuffer(&IndexBufferView);
	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart());
	for (const auto& MeshName : NameMeshDir)
	{
		auto count = mMeshGeo->DrawArgs[MeshName.first].IndexCount;
		mCommandList->DrawIndexedInstanced(
			mMeshGeo->DrawArgs[MeshName.first].IndexCount,
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
		float dx = -1.0f * XMConvertToRadians(mSensitivity * static_cast<float>(x - mLastMousePos.x));
		float dy = -1.0f * XMConvertToRadians(mSensitivity * static_cast<float>(y - mLastMousePos.y));

		// update angles based on input to orbit camera around box;
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi
		mPhi = FMathHelper::Clamp(mPhi, 0.1f, FMathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene
		float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);

		// update the camera radius based on input
		mRadius += dx - dy;

		// Restrict the radius
		mRadius = FMathHelper::Clamp(mRadius, 30.0f, 150.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void FGameProcess::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&mCbvHeap)));
}
void FGameProcess::BulidConstantBuffers()
{
	mObjectCB = std::make_unique<UploadBuffer<Charalotte::ObjectConstants>>(md3dDevice.Get(), 1, true);

	UINT objCBByteSize = FUtil::CalcConstantBufferByteSize(sizeof(Charalotte::ObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.

	// why we use 0??
	int boxCBufIndex = 0;

	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = FUtil::CalcConstantBufferByteSize(sizeof(Charalotte::ObjectConstants));

	md3dDevice->CreateConstantBufferView(
		&cbvDesc,
		mCbvHeap->GetCPUDescriptorHandleForHeapStart());
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
	FDataProcessor::LoadMesh(GeometryName, MeshInfo);
	std::string Name = GeometryName;
	if (MeshInfo.LodInfos.size() <= 0)
	{
		return;
		std::stringstream ss;
		ss << GeometryName << "No result";
		OutputDebugStringA(ss.str().c_str());
	}
	auto ArrayData = MeshInfo.LodInfos[0].VerticesLocation.data();
	XMFLOAT4 VertexColor;
	if (ColorIndex < TestColors.size())
	{
		VertexColor = TestColors[ColorIndex];
		ColorIndex++;
	}
	else
	{
		ColorIndex = 0;
		VertexColor = TestColors[ColorIndex];
	}

	for (const auto& VertexLocation : MeshInfo.LodInfos[0].VerticesLocation)
	{
		Charalotte::Vertex vertex;
		XMFLOAT3 Float3;
		XMMATRIX Transport;
		// now we only use translation, because i have some idea to deal with the 
		// rotation and scale but i need some time to come true

		// execute scale transport
		XMMATRIX ScaleTrans = XMMatrixScaling(Transform.Scale3D.x, Transform.Scale3D.y, Transform.Scale3D.z);
		XMFLOAT4 NowLocation{ VertexLocation.x, VertexLocation.y, VertexLocation.z, 1.0f };
		XMVECTOR MeshLocationVector = XMLoadFloat4(&NowLocation);

		// execute rotate transport
		XMMATRIX RotateTrans = XMMatrixRotationRollPitchYaw(Transform.Rotation.y, Transform.Rotation.z, Transform.Rotation.x);
		Transport = ScaleTrans;

		MeshLocationVector = FMathHelper::VectorMultipyMatrix(MeshLocationVector, XMMatrixTranspose(Transport));
		XMStoreFloat4(&NowLocation, MeshLocationVector);

		// execute  transport
		Float3.x = (NowLocation.x + Transform.Translation.x) / 100.0f;
		Float3.y = (NowLocation.y + Transform.Translation.y) / 100.0f;
		Float3.z = (NowLocation.z + Transform.Translation.z) / 100.0f;

		vertex.Pos = Float3;
		vertex.Color = VertexColor;
		vertices.push_back(vertex);
	}

	for (const auto& index : MeshInfo.LodInfos[0].Indices)
	{
		int32_t VertexIndex = index;
		VertexIndex += VertexNumDrawed;
		indices.push_back(static_cast<int16_t>(VertexIndex));
	}
	VertexNumDrawed += MeshInfo.LodInfos[0].VerticesNum;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)(MeshInfo.LodInfos[0].Indices.size());
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	if (Name.size() <= 4)
	{
		return;
	}
	std::string RealActorName = Name.erase(Name.size() - 4, 4);
	if (NameMeshDir.find(RealActorName) != NameMeshDir.end())
	{
		auto RepeatNameIter = RepeatName.find(RealActorName);
		uint32_t RepeatActorAdd = 0;;
		if (RepeatNameIter != RepeatName.end())
		{
			RepeatNameIter->second += 1;
			RepeatActorAdd = RepeatNameIter->second;
		}
		else
		{
			RepeatName.insert(std::make_pair(RealActorName, 0));
		}
		RealActorName += std::to_string(static_cast<int>(RepeatActorAdd)) ;
	}
	NameMeshDir.insert(std::make_pair(RealActorName, submesh));
}

void FGameProcess::BuildMeshGeometrys()
{
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Charalotte::Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(int);


	mMeshGeo = std::make_unique<MeshGeometry>();
	mMeshGeo->Name = "meshGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &mMeshGeo->VertexBufferCPU));
	CopyMemory(mMeshGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &mMeshGeo->IndexBufferCPU));
	CopyMemory(mMeshGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	mMeshGeo->VertexBufferGPU = FUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, mMeshGeo->VertexBufferUploader);

	mMeshGeo->IndexBufferGPU = FUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, mMeshGeo->IndexBufferUploader);

	mMeshGeo->VertexByteStride = sizeof(Charalotte::Vertex);
	mMeshGeo->VertexBufferByteSize = vbByteSize;
	mMeshGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	mMeshGeo->IndexBufferByteSize = ibByteSize;

	for (const auto& MeshNameDir : NameMeshDir)
	{
		mMeshGeo->DrawArgs[MeshNameDir.first] = MeshNameDir.second;
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

void FGameProcess::TestFunc()
{
	XMMATRIX Test = XMMatrixTranslation(1.0F, 2.0F, 3.0F);
}




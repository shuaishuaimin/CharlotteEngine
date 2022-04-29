#include "stdafx.h"
#ifdef RENDER_PLATFORM_DX12
#include "FDXShadowMap.h"
#include "FDXResource.h"
#include "FHeapManager.h"
namespace Charalotte
{
	FDXShadowMap::FDXShadowMap(
		UINT width, UINT height, FDevice* Device, FHeapManager* HeapPtr) : mWidth(width), mHeight(height), Device(Device), HeapMgrPtr(HeapPtr)
	{
	}

	FDXShadowMap::~FDXShadowMap()
	{
		mShadowMap = nullptr;
		if (HeapMgrPtr != nullptr)
		{
			HeapMgrPtr->ReleaseOffset(HeapType::CBVSRVUAVHeap, SrvHeapOffset);
			HeapMgrPtr->ReleaseOffset(HeapType::DSVHeap, DsvHeapOffset);
		}
	}
	void FDXShadowMap::Init()
	{
		BuildShadowMapResource(this->Device);
		FDXDevice* DxDevice = dynamic_cast<FDXDevice*>(Device);
		auto DevicePtr = DxDevice->GetDevice();
		if (HeapMgrPtr == nullptr)
		{
			return;
		}
		SrvHeapOffset = HeapMgrPtr->GetAvailableOffsetAndUseIt(HeapType::CBVSRVUAVHeap);
		DsvHeapOffset = HeapMgrPtr->GetAvailableOffsetAndUseIt(HeapType::DSVHeap);
		CreateSRVAndDSV(DevicePtr);
	}

	void FDXShadowMap::OnResize(UINT newWidth, UINT newHeight)
	{
		if ((mWidth != newWidth) || (mHeight != newHeight))
		{
			mWidth = newWidth;
			mHeight = newHeight;

			BuildShadowMapResource(this->Device);
		}
	}

	void FDXShadowMap::BuildShadowMapResource(FDevice* Device)
	{
		D3D12_RESOURCE_DESC texDesc;
		SecureZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = mWidth;
		texDesc.Height = mHeight;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = mFormat;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		FDXDevice* DxDevice = dynamic_cast<FDXDevice*>(Device);
		auto DevicePtr = DxDevice->GetDevice();
		auto HeapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		ThrowIfFailed(DevicePtr->CreateCommittedResource(
			&HeapPro,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optClear,
			IID_PPV_ARGS(&mShadowMap)));
		mShadowMap->SetName(L"shadowmap");
		ResourcePtr = std::make_shared<FDXResource>(DevicePtr);
		dynamic_cast<FDXResource*>(ResourcePtr.get())->GetResource() = mShadowMap;
	}

	void FDXShadowMap::CreateSRVAndDSV(ID3D12Device* device)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
		srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MostDetailedMip = 0;
		srv.Texture2D.MipLevels = 1;
		srv.Texture2D.ResourceMinLODClamp = 0.0f;
		srv.Texture2D.PlaneSlice = 0;
		device->CreateShaderResourceView(mShadowMap.Get(), &srv, HeapMgrPtr->GetCPUHandleByTypeAndOffest(HeapType::CBVSRVUAVHeap, SrvHeapOffset));
		mSrv = srv;

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv;
		dsv.Flags = D3D12_DSV_FLAG_NONE;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsv.Texture2D.MipSlice = 0;
		device->CreateDepthStencilView(mShadowMap.Get(), &dsv, HeapMgrPtr->GetCPUHandleByTypeAndOffest(HeapType::DSVHeap, DsvHeapOffset));
		mDsv = dsv;
	}

	ID3D12Resource* FDXShadowMap::GetResource()
	{
		return mShadowMap.Get();
	}

}

#endif
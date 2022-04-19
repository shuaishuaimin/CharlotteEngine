#include "stdafx.h"
#include "FDXResource.h"

namespace Charalotte
{
	void FDXResource::BuildResource(FResourceAttributes ResourceA, FHeapManager* HeapMgr)
	{
		switch (ResourceA.ResourceType)
		{
		case Charalotte::E_RESOURCE_TYPE::RESOURCE_DEP:
			BuildDsResource(ResourceA, HeapMgr);
			break;
		case Charalotte::E_RESOURCE_TYPE::RESOURCE_SHADER:
			break;
		case Charalotte::E_RESOURCE_TYPE::RESOURCE_RT:
			BuildRTResource(ResourceA, HeapMgr);
			break;
		default:
			break;
		}
	}
	void FDXResource::BuildRTResource(FResourceAttributes ResourceA, FHeapManager* HeapMgr)
	{
		const FLOAT Color[4] = { 0.0f,0.0f,0.0f,0.0f };
		CD3DX12_CLEAR_VALUE optClear(DXGI_FORMAT(ResourceA.Format), Color);

		D3D12_RESOURCE_DESC RTdesc;
		RTdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		RTdesc.Alignment = 0;
		RTdesc.Width = UINT(ResourceA.Width);
		RTdesc.Height = UINT(ResourceA.Height);
		RTdesc.DepthOrArraySize = 1;
		RTdesc.MipLevels = 1;
		RTdesc.Format = DXGI_FORMAT(ResourceA.Format);
		RTdesc.SampleDesc.Count = false ? 4 : 1;
		RTdesc.SampleDesc.Quality = false ? (0 - 1) : 0;
		RTdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		RTdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		optClear.Format = DXGI_FORMAT(ResourceA.Format);
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		optClear.Color[0] = Color[0];
		optClear.Color[1] = Color[1];
		optClear.Color[2] = Color[2];
		optClear.Color[3] = Color[3];

		auto HeapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(DevicePtr->CreateCommittedResource(
			&HeapPro,
			D3D12_HEAP_FLAG_NONE,
			&RTdesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&optClear,
			IID_PPV_ARGS(mResource.GetAddressOf())));

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = DXGI_FORMAT(ResourceA.Format);
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		auto rtvHeandle = HeapMgr->GetCPUHandleByTypeAndOffest(Charalotte::HeapType::RTVHeap, ResourceA.RtO);
		DevicePtr->CreateRenderTargetView(mResource.Get(), &rtvDesc, rtvHeandle);
		
		//create srv view
		D3D12_SHADER_RESOURCE_VIEW_DESC rtsrvDesc = {};
		rtsrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		rtsrvDesc.Format = DXGI_FORMAT(ResourceA.Format);
		rtsrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		rtsrvDesc.Texture2D.MostDetailedMip = 0;
		rtsrvDesc.Texture2D.MipLevels = 1;
		rtsrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		rtsrvDesc.Texture2D.PlaneSlice = 0;
		auto SrvHandle = HeapMgr->GetCPUHandleByTypeAndOffest(Charalotte::HeapType::CBVSRVUAVHeap, ResourceA.SrO);
		DevicePtr->CreateShaderResourceView(mResource.Get(), &rtsrvDesc, SrvHandle);
	}
	void FDXResource::BuildDsResource(FResourceAttributes ResourceA, FHeapManager* HeapMgr)
	{
		const FLOAT Color[4] = { 0.0f,0.0f,0.0f,0.0f };
		CD3DX12_CLEAR_VALUE optClear(DXGI_FORMAT(ResourceA.Format), Color);
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = ResourceA.Width;
		depthStencilDesc.Height = ResourceA.Height;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthStencilDesc.SampleDesc.Count = false ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = false ? (0 - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		auto HeapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(DevicePtr->CreateCommittedResource(
			&HeapPro,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optClear,
			IID_PPV_ARGS(mResource.GetAddressOf())));

		// create dsv view
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.Texture2D.MipSlice = 0;

		auto DsvHandle = HeapMgr->GetCPUHandleByTypeAndOffest(HeapType::DSVHeap, ResourceA.DsO);
		DevicePtr->CreateDepthStencilView(mResource.Get(), &dsvDesc, DsvHandle);

		D3D12_SHADER_RESOURCE_VIEW_DESC dssrvDesc = {};
		dssrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		dssrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		dssrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		dssrvDesc.Texture2D.MostDetailedMip = 0;
		dssrvDesc.Texture2D.MipLevels = 1;
		dssrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		dssrvDesc.Texture2D.PlaneSlice = 0;

		auto DsSrvHandle = HeapMgr->GetCPUHandleByTypeAndOffest(HeapType::CBVSRVUAVHeap, ResourceA.SrO);
		DevicePtr->CreateShaderResourceView(mResource.Get(), &dssrvDesc, DsSrvHandle);
	}
}


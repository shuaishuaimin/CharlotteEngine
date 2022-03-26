#include "stdafx.h"
#include "FDXShadowMap.h"

FDXShadowMap::FDXShadowMap(
	UINT width, UINT height, FDevice* Device) : mWidth(width), mHeight(height), Device(Device)
{
	mShadowSrvHeap = nullptr;
	BuildShadowMapResource(this->Device);
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
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&mShadowMap)));
	mShadowMap->SetName(L"shadowmap");
}

void FDXShadowMap::BuildShadowMapDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv)
{
	mhCpuSrv = hCpuSrv;
	mhGpuSrv = hGpuSrv;
	mhCpuDsv = hCpuDsv;
	FDXDevice* DxDevice = dynamic_cast<FDXDevice*>(Device);
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;
	DxDevice->GetDevice()->CreateShaderResourceView(mShadowMap.Get(), &srvDesc, mhCpuSrv);

	//Create DSV to resource so we can render to the shadow map
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	DxDevice->GetDevice()->CreateDepthStencilView(mShadowMap.Get(), &dsvDesc, mhCpuDsv);
}

ID3D12Resource* FDXShadowMap::GetResource()
{
	return mShadowMap.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE FDXShadowMap::GetDsv() const 
{
	return mhCpuDsv;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& FDXShadowMap::GetSrvHeap()
{
	return mShadowSrvHeap;
}
#pragma once
#include "windows.h"
#include "wrl.h"
#include "d3dx12.h"
#include "FUtil.h"
#include "FDXDevice.h"

class FDXShadowMap
{
public:
	FDXShadowMap(UINT width, UINT height, FDevice* Device);
	
	FDXShadowMap(const FDXShadowMap& rhs) = delete;
	FDXShadowMap& operator= (const FDXShadowMap& rhs) = delete;
	FDXShadowMap(FDXShadowMap&& rhs) = delete;
	~FDXShadowMap() = default;

	void OnResize(UINT newWidth, UINT newHeight);

	void BuildShadowMapResource(FDevice* Device);
	
	void BuildShadowMapDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv);

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv() const;

	ID3D12Resource* GetResource();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetSrvHeap();
protected:
	
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
	FDevice* Device;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuDsv;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mShadowSrvHeap;

	DXGI_FORMAT mFormat = DXGI_FORMAT_R24G8_TYPELESS;

	UINT mWidth;
	UINT mHeight;
};
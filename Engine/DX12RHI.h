#pragma once
#include <windows.h>
#include <wrl.h>
#include <memory>
#include "d3dx12.h"
#include "RHI.h"
#include "WinMaterialData.h"
#include "FUtil.h"

class DX12RHI : public RHI
{
public:
	DX12RHI() {}
	virtual ~DX12RHI() {}

	virtual void LoadTextureAsset(const std::string& FileName) override;

protected:
	std::string wString2String(const std::wstring& ws) {}
	std::wstring String2wString(const std::string& s) {}
private:

	// render ptr
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	// command 
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
};

void DX12RHI::LoadTextureAsset(const std::string& FileName)
{
	auto Texture = std::make_shared<Charalotte::DXTextureResource>();
	Texture->Name = FileName;
	Texture->Filename = L"Content/Textures/bricks.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
		mCommandList.Get(), Texture->Filename.c_str(),
		Texture->Resource, Texture->UploadHeap));

	FDXResources::GetInstance().AddDxTextureResource(Texture->Name, Texture);
}
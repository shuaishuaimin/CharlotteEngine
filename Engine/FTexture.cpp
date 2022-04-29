#include "stdafx.h"
#include "FHeapManager.h"
#include "FTexture.h"
#ifdef RENDER_PLATFORM_DX12
#include "FHeapManager.h"
#endif

namespace Charalotte
{
// You must not set offset. Every using this code must use heap manager to give handle offset; 
#ifdef  RENDER_PLATFORM_DX12
	FTexture::FTexture(const std::string& Path, FHeapManager* Mgr) : TexturePath(Path)
	{
		Offsets = {
			{HeapType::CBVSRVUAVHeap, Mgr->GetAvailableOffsetAndUseIt(HeapType::CBVSRVUAVHeap)},
			{HeapType::DSVHeap, Mgr->GetAvailableOffsetAndUseIt(HeapType::DSVHeap)},
			{HeapType::RTVHeap, Mgr->GetAvailableOffsetAndUseIt(HeapType::RTVHeap)}
		};
	}
	#else
	FTexture(const std::string& Path) : TexturePath(Path)
	{

	}
	#endif
	FTexture::~FTexture() {}
	std::string FTexture::GetTexturePath()
	{
		return TexturePath;
	}

	void FTexture::SetTexturePath(const std::string& Path)
	{
		TexturePath = Path;
	}

	#ifdef RENDER_PLATFORM_DX12
	Microsoft::WRL::ComPtr<ID3D12Resource>& FTexture::GetResource()
	{
		return Resource;
	}
	Microsoft::WRL::ComPtr<ID3D12Resource>& FTexture::GetUploadHeap()
	{
		return UploadHeap;
	}
	#endif

	void FTexture::ClearTexture()
	{
	#ifdef RENDER_PLATFORM_DX12
		Resource = nullptr;
		UploadHeap = nullptr;
	#endif
	}
}
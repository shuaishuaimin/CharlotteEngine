#pragma once
#include <string>
#include "DDefines.h"
#ifdef RENDER_PLATFORM_DX12
#include "d3dx12.h"
#include "wrl.h"
#endif // 

namespace Charalotte
{
	class FTexture
	{
	public:
		FTexture(const std::string& Path) : TexturePath(Path)
		{

		}

		~FTexture() {}
		std::string GetTexturePath()
		{
			return TexturePath;
		}

		void SetTexturePath(const std::string& Path)
		{
			TexturePath = Path;
		}

#ifdef RENDER_PLATFORM_DX12
		inline Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource()
		{
			return Resource;
		}
		inline Microsoft::WRL::ComPtr<ID3D12Resource>& GetUploadHeap()
		{
			return UploadHeap;
		}
#endif

		void ClearTexture()
		{
#ifdef RENDER_PLATFORM_DX12
			Resource = nullptr;
			UploadHeap = nullptr;
#endif
		}

	private:
		std::string TexturePath;
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
#endif
	};
}

#pragma once
#include <string>
#include "DDefines.h"
#include "EHeapType.h"
#ifdef RENDER_PLATFORM_DX12
#include "d3dx12.h"
#include "wrl.h"
#endif // 

namespace Charalotte
{
	class FHeapManager;
	class FTexture
	{
	public:
		FTexture(const std::string& Path) : TexturePath(Path)
		{

		}
#ifdef RENDER_PLATFORM_DX12
		FTexture(const std::string& Path, FHeapManager* Mgr);
#else
		FTexture(const std::string& Path) : TexturePath(Path)
		{
			
		}
#endif
		~FTexture();
		std::string GetTexturePath();
		
		void SetTexturePath(const std::string& Path);

		int GetOffset(HeapType Type)
		{
			const auto& Iter = Offsets.find(Type);
			if (Iter != Offsets.end())
			{
				return Iter->second;
			}
			else
			{
				return -1;
			}
		}
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource();

		Microsoft::WRL::ComPtr<ID3D12Resource>& GetUploadHeap();
#endif
		void ClearTexture();

	private:
		std::string TexturePath;
		std::unordered_map<HeapType, int> Offsets;
#ifdef RENDER_PLATFORM_DX12
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
#endif
	};
}

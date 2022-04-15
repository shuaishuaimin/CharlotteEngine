#pragma once
#include "DDefines.h"
#ifdef RENDER_PLATFORM_DX12
#include <windows.h>
#include <wrl.h>
#include "d3dx12.h"
#endif

namespace Charalotte
{
	class FVerticesAndIndicesBuffer
	{
	public:
		FVerticesAndIndicesBuffer() {}
		~FVerticesAndIndicesBuffer() {}

		using index_type_t = int16_t;

		void CreateRenderPlatformResource()
		{
			
		}

		inline UINT GetVerticesSize()
		{
			return (UINT)vertices.size() * sizeof(Charalotte::Vertex);
		}

		inline UINT GetIndicesSize()
		{
			return (UINT)indices.size() * sizeof(index_type_t);
		}
		inline UINT GetIndicesLength()
		{
			return (UINT)indices.size();
		}

		inline void InsertVertex(Vertex& vertex)
		{
			vertices.push_back(std::move(vertex));
		}

		inline void InsertIndices(index_type_t Index)
		{
			indices.push_back(Index);
		}

		inline void SetVBIBName(const std::string& Name)
		{
			VBIBName = Name;
		}

		inline std::string GetName()
		{
			return VBIBName;
		}

		void ClearBuffers();
#ifdef RENDER_PLATFORM_DX12
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const;

		D3D12_INDEX_BUFFER_VIEW IndexBufferView()const;

#endif
	private:
		std::string VBIBName;
		std::vector<Charalotte::Vertex> vertices;
		std::vector<index_type_t> indices;
		UINT VertexByteStride = 0;
		UINT VertexBufferByteSize = 0;
		UINT IndexBufferByteSize = 0;
#ifdef RENDER_PLATFORM_DX12
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;
#endif
	};
}


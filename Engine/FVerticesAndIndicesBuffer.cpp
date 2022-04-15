#include "stdafx.h"
#include "FVerticesAndIndicesBuffer.h"

namespace Charalotte
{
#ifdef RENDER_PLATFORM_DX12
	D3D12_VERTEX_BUFFER_VIEW FVerticesAndIndicesBuffer::VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW FVerticesAndIndicesBuffer::IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void FVerticesAndIndicesBuffer::ClearBuffers()
	{
		VertexBufferCPU = nullptr;
		IndexBufferCPU = nullptr;

		VertexBufferGPU = nullptr;
		IndexBufferGPU = nullptr;

		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
#endif
}


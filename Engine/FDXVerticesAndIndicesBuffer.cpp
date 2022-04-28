#include "stdafx.h"
#include "FDXVerticesAndIndicesBuffer.h"

namespace Charalotte
{
#ifdef RENDER_PLATFORM_DX12
	D3D12_VERTEX_BUFFER_VIEW FDXVerticesAndIndicesBuffer::VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW FDXVerticesAndIndicesBuffer::IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void FDXVerticesAndIndicesBuffer::ClearBuffers()
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


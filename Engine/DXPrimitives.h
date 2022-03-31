#pragma once
#include "stdafx.h"
#include "STransformDef.h"
#include "FUploadBuffer.h"
#include "WinMaterialData.h"
#include "FMaterial.h"

namespace Charalotte
{

	enum WindowsAction
	{
		Move,
		MouseDown,
		MouseUp
	};
	struct WindowsInputData
	{
		WPARAM btnState;
		int x;
		int y;
		WindowsAction Action;
	};

	// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
	// geometries are stored in one vertex and index buffer.  It provides the offsets
	// and data needed to draw a subset of geometry stores in the vertex and index 
	// buffers so that we can implement the technique described by Figure 6.3.
	struct FDXSubmeshPrimitive
	{
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		INT BaseVertexLocation = 0;

		// Bounding box of the geometry defined by this submesh. 
		// This is used in later chapters of the book.
		DirectX::BoundingBox Bounds;
	};

	struct FDXMeshPrimitive
	{
	
		using index_type_t = int16_t;
		// Give it a name so we can look it up by name.
		std::string Name;
		std::vector<Charalotte::Vertex> vertices;
		std::vector<index_type_t> indices;

		// System memory copies.  Use Blobs because the vertex/index format can be generic.
		// It is up to the client to cast appropriately.  
		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

		// Data about the buffers.
		UINT VertexByteStride = 0;
		UINT VertexBufferByteSize = 0;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		UINT IndexBufferByteSize = 0;

		// A MeshGeometry may store multiple geometries in one vertex/index buffer.
		// Use this container to define the Submesh geometries so we can draw
		// the Submeshes individually.
		std::unordered_map<std::string, FDXSubmeshPrimitive> DrawArgs;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
		{
			D3D12_VERTEX_BUFFER_VIEW vbv;
			vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
			vbv.StrideInBytes = VertexByteStride;
			vbv.SizeInBytes = VertexBufferByteSize;

			return vbv;
		}

		D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
		{
			D3D12_INDEX_BUFFER_VIEW ibv;
			ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
			ibv.Format = IndexFormat;
			ibv.SizeInBytes = IndexBufferByteSize;

			return ibv;
		}

		// We can free this memory after we finish upload to the GPU.
		void DisposeUploaders()
		{
			VertexBufferUploader = nullptr;
			IndexBufferUploader = nullptr;
		}

		UINT GetVerticesSize()
		{
			return (UINT)vertices.size() * sizeof(Charalotte::Vertex);
		}

		UINT GetIndicesSize()
		{
			return (UINT)indices.size() * sizeof(Charalotte::FDXMeshPrimitive::index_type_t);
		}
	};

	struct FDXActorPrimitive
	{
		std::string DXActorPrimitiveName;
		Charalotte::FDXMeshPrimitive* DXMeshPrimitive = nullptr;
		Charalotte::FTransform Transform;
		std::shared_ptr<UploadBuffer<Charalotte::ObjectConstants>> ObjectCB = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SrvHeap = nullptr;
		glm::mat4 VPTrans = glm::mat4(1.0f);

		glm::mat4 MTrans = glm::mat4(1.0f);

		FMaterial* Material = nullptr;
	};
}


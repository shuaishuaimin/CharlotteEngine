#pragma once
#include <memory>
#include "FVerticesAndIndicesBuffer.h"
#include "FMaterial.h"

namespace Charalotte
{
	class FRenderMesh
	{
	public:
		FRenderMesh()
		{

		}
		void SetMaterial(FMaterial* Material)
		{

		}

	private:
		std::unique_ptr<FVerticesAndIndicesBuffer> VBIB;
		FMaterial* Material; 
	};
}

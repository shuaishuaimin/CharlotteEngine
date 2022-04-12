#pragma once
#include <memory>
#include "FVerticesAndIndicesBuffer.h"

namespace Charalotte
{
	class FRenderMesh
	{
	public:
		FRenderMesh()
		{

		}


	private:
		std::unique_ptr<FVerticesAndIndicesBuffer> VBIB;
	};
}

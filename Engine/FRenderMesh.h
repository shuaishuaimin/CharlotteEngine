#pragma once

namespace Charalotte
{
	class FMaterial;
	class FRenderMesh
	{
	public:
		FRenderMesh(){}
		virtual ~FRenderMesh(){}

		virtual FMaterial* GetMaterialPtr() = 0;
	};
}
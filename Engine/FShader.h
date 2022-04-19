#pragma once
#include <memory>
#include "SEShaderElements.h"
#include "RHI.h"
#include "DDefines.h"
#ifdef RENDER_PLATFORM_DX12
#include "SDX12ShaderElement.h" 
#endif
namespace Charalotte
{
	class FShader
	{
	public:
		FShader(const std::string& Path)
		{
			Attributes = nullptr;
			shaderPath = Path;


#ifdef RENDER_PLATFORM_DX12
			ShaderEle = std::make_shared<FDXShaderElement>();
#endif
		}
		virtual ~FShader()
		{
			Attributes = nullptr;
		}
		std::shared_ptr<FVSPSAttributes>& GetAttributes()
		{
			return Attributes;
		}
		void SetAttributes(std::shared_ptr<Charalotte::FVSPSAttributes> SI)
		{
			Attributes = SI;
		}
		std::string& ShaderPath()
		{
			return shaderPath;
		}

		virtual void SetRarameter(RHI* R) = 0;
#ifdef RENDER_PLATFORM_DX12
		void SetShaderElement(std::shared_ptr<FDXShaderElement>& Ele)
		{
			ShaderEle = Ele;
		}
		FDXShaderElement* GetShaderElement()
		{
			return ShaderEle.get();
		}
#endif
	private:
		std::string shaderPath;
		std::shared_ptr<Charalotte::FVSPSAttributes> Attributes;
#ifdef RENDER_PLATFORM_DX12
		std::shared_ptr<FDXShaderElement> ShaderEle;
#endif
	};

}


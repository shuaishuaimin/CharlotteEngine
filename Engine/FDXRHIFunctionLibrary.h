#pragma once
#include "DDefines.h"
#ifdef RENDER_PLATFORM_DX12
#include <string>
#include <codecvt>
#include <windows.h>
#include <d3dx12.h>
#include "SEShaderElements.h"

class FDXRHIFunctionLibrary
{
public:
	FDXRHIFunctionLibrary(){}
	~FDXRHIFunctionLibrary(){}

	static std::string wString2String(const std::wstring& ws)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		return myconv.to_bytes(ws);
	}
	static std::wstring String2wString(const std::string& s)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		return myconv.from_bytes(s);
	}
	// dangerous code
	static std::shared_ptr<D3D_SHADER_MACRO> ShaderMacro2DX12(Charalotte::FShaderMacro* ShaderMacro)
	{
		std::shared_ptr<D3D_SHADER_MACRO> ShaderMacroPtr = std::make_shared<D3D_SHADER_MACRO>();
		ShaderMacroPtr->Definition = LPCSTR(String2wString(ShaderMacro->Definetion).c_str());
		ShaderMacroPtr->Name = LPCSTR(String2wString(ShaderMacro->Name).c_str());
		return ShaderMacroPtr;
	}
	static std::vector<D3D12_INPUT_ELEMENT_DESC> InputDescS2DX12(const std::vector<>)
};

#endif
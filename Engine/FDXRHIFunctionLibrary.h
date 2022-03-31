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
		if (ws.empty())
		{
			return "";
		}
		std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		return myconv.to_bytes(ws);
	}
	static std::wstring String2wString(const std::string& s)
	{
		if (s.empty())
		{
			return L"";
		}
		std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		return myconv.from_bytes(s);
	}
	// dangerous code
	static std::shared_ptr<D3D_SHADER_MACRO> ShaderMacro2DX12(Charalotte::FShaderMacro* ShaderMacro)
	{
		if (ShaderMacro == nullptr)
		{
			return nullptr;
		}
		std::shared_ptr<D3D_SHADER_MACRO> ShaderMacroPtr = std::make_shared<D3D_SHADER_MACRO>();
		std::string Definetion = ShaderMacro->Definetion;
		std::string Name = ShaderMacro->Name;
		ShaderMacroPtr->Definition = LPCSTR(std::move(Definetion).c_str());
		ShaderMacroPtr->Name = LPCSTR(std::move(Name).c_str());
		return ShaderMacroPtr;
	}
	static std::vector<D3D12_INPUT_ELEMENT_DESC> InputDescS2DX12(std::vector<Charalotte::FInputElementDesc>& InputElements)
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> ResultElements;
		for (const auto& InputElement : InputElements)
		{
			D3D12_INPUT_ELEMENT_DESC InputLayout;
			InputLayout.AlignedByteOffset = InputElement.ByteOffest;
			InputLayout.Format = DXGI_FORMAT(InputElement.Format);
			InputLayout.InputSlot = InputElement.InputSlot;
			InputLayout.InputSlotClass = D3D12_INPUT_CLASSIFICATION(InputElement.InputSlotClass);
			InputLayout.InstanceDataStepRate = InputElement.InstanceDataStepRate;
			InputLayout.SemanticIndex = InputElement.Index;
			auto NamePtrCopy = InputElement.Name.c_str();
			InputLayout.SemanticName = LPCSTR(std::move(NamePtrCopy));
			ResultElements.push_back(std::move(InputLayout));
		}
		return ResultElements;
	}
};

#endif
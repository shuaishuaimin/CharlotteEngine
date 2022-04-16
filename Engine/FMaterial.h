#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "SMaterialAttributes.h"
#include "SEShaderElements.h"
#include "FRenderPSO.h"
#include "FTexture.h"

namespace Charalotte
{
	class FMaterial
	{
	public:
		FMaterial() : mTextureName("bricks"), mNormalName("bricks")
		{
			MaterialAttributes = std::make_unique<FMaterialAttributes>();
			EmptyPsos = {};
		}

		~FMaterial() {
			ClearMaterial();
		}
		void SetMaterialAttributes(const FMaterialAttributes& Attributes) {
			MaterialAttributes->BaseColor = Attributes.BaseColor;
			MaterialAttributes->Normal = Attributes.Normal;
			MaterialAttributes->Roughness = Attributes.Roughness;
		}

		inline void SetTexture(const std::string& TextureName)
		{
			mTextureName = TextureName;
		}

		std::string GetTexture() const
		{
			return mTextureName;
		}

		inline void SetNormal(const std::string& NormalName)
		{
			mNormalName = NormalName;
		}

		inline void SetNormal(FTexture* NormalTex)
		{
			NormalTexture = NormalTex;
		}

		inline void SetTexture(FTexture* Tex)
		{
			Texture = Tex;
		}

		inline FTexture* GetTexturePtr()
		{
			return Texture;
		}

		inline FTexture* GetNormalPtr()
		{
			return NormalTexture;
		}
		std::string GetNormal() const
		{
			return mNormalName;
		}

		std::vector<std::shared_ptr<FRenderPSO>>& GetPSOs(E_PSOTYPE psotype)
		{
			const auto& Iter = Psos.find(psotype);
			if (Iter != Psos.end())
			{
				return Iter->second;
			}
			else
			{
				return EmptyPsos;
			}
		}

		void ClearMaterial()
		{
			MaterialAttributes = nullptr;
			for (auto& Pso : Psos)
			{
				for (auto& RenderPSO : Pso.second)
				{
					RenderPSO = nullptr;
				}
			}
			Psos.clear();
			for (auto& EmptyPso : EmptyPsos)
			{
				EmptyPso = nullptr;
			}
		}
	private:
		std::string mTextureName;
		std::string mNormalName;
		FTexture* Texture;
		FTexture* NormalTexture;
		std::unique_ptr<FMaterialAttributes> MaterialAttributes;
		std::unordered_map<E_PSOTYPE, std::vector<std::shared_ptr<FRenderPSO>>> Psos;
		std::vector<std::shared_ptr<FRenderPSO>> EmptyPsos;
	};
}

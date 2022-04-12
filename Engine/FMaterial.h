#pragma once
#include <memory>
#include <vector>
#include <string>
#include "SMaterialAttributes.h"

namespace Charalotte
{
	class FMaterial
	{
	public:
		FMaterial() : mTextureName("bricks"), mNormalName("bricks")
		{
			MaterialAttributes = std::make_unique<FMaterialAttributes>();
		}

		~FMaterial() {
			MaterialAttributes = nullptr;
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

		std::string GetNormal() const
		{
			return mNormalName;
		}
	private:
		std::string mTextureName;
		std::string mNormalName;
		std::unique_ptr<FMaterialAttributes> MaterialAttributes;
	};
}

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
		}

		~FMaterial() {
			ClearMaterial();
		}
		void SetMaterialAttributes(const FMaterialAttributes& Attributes) {
			MaterialAttributes->BaseColor = Attributes.BaseColor;
			MaterialAttributes->Normal = Attributes.Normal;
			MaterialAttributes->Roughness = Attributes.Roughness;
		}
		FMaterialAttributes* GetAttributes()
		{
			return MaterialAttributes.get();
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

		void AddPSO(FRenderPSO* PsoPtr, const std::string& PsoName, E_PSOTYPE type)
		{
			const auto& Iter = Psos.find(type);
			if (Iter != Psos.end())
			{
				Iter->second.insert({PsoName, PsoPtr});
			}
			else
			{
				Psos.insert({type, {{PsoName, PsoPtr}}});
			}
		}
		FRenderPSO* GetPso(const std::string& PsoName, E_PSOTYPE type)
		{
			const auto& TypeIter = Psos.find(type);
			if (TypeIter != Psos.end())
			{
				const auto& PsoIter = TypeIter->second.find(PsoName);
				if (PsoIter != TypeIter->second.end())
				{
					return PsoIter->second;
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
		}

		void ClearMaterial()
		{
			MaterialAttributes = nullptr;
			for (auto& Pso : Psos)
			{
				Pso.second.clear();
			}
			Psos.clear();
		}
	private:
		std::string mTextureName;
		std::string mNormalName;
		std::unique_ptr<FMaterialAttributes> MaterialAttributes;
		std::unordered_map<E_PSOTYPE, std::unordered_map<std::string, FRenderPSO*>> Psos;
	};
}

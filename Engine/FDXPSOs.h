#pragma once
#include <string>
#include "d3dx12.h"
#include "DX12RHIData.h"
#include "BaseStructAllPlatform.h"

namespace Charalotte
{
	class FDXPSOs
	{
	public:
		FDXPSOs() {
			PSOs.insert({ Charalotte::Default, {} });
			PSOs.insert({ Charalotte::Shadow, {} });
			EmptyPSO = {};
		}

		inline void InsertNewPSO(Charalotte::PSOType psoType, Charalotte::PSO& Pso)
		{
			PSOs.insert({ psoType, std::move(Pso) });
		}

		inline void RemovePSO(Charalotte::PSOType psoType)
		{
			PSOs.erase(psoType);
		}

		Charalotte::PSO& GetPSOReference(Charalotte::PSOType psoType, bool& IsGetSuccess)
		{
			const auto& Iter = PSOs.find(psoType);
			if (Iter != PSOs.end())
			{
				IsGetSuccess = true;
				return Iter->second;
			}
			else
			{
				IsGetSuccess = false;
				return EmptyPSO;
			}
		}
		Charalotte::PSO& GetPSOReference(Charalotte::PSOType psoType)
		{
			const auto& Iter = PSOs.find(psoType);
			if (Iter != PSOs.end())
			{
				return Iter->second;
			}
			else
			{
				return EmptyPSO;
			}
		}

		Charalotte::PSO GetPSOCopy(Charalotte::PSOType psoType)
		{
			const auto& Iter = PSOs.find(psoType);
			if (Iter != PSOs.end())
			{
				return Iter->second;
			}
			else
			{
				return EmptyPSO;
			}
		}
		std::unordered_map<Charalotte::PSOType, Charalotte::PSO>& GetPSOs()
		{
			return PSOs;
		}
	private:
		std::unordered_map<Charalotte::PSOType, Charalotte::PSO> PSOs;
		Charalotte::PSO EmptyPSO;
	};
}

#pragma once
#include <string>
#include "d3dx12.h"
#include "DX12RHIData.h"
#include "STransformDef.h"

namespace Charalotte
{
	class FDXPSOs
	{
	public:
		FDXPSOs() {
			EmptyPSO = {};
		}

		inline void InsertNewPSO(Charalotte::E_PSOTYPE psoType, const Charalotte::PSO& Pso)
		{
			PSOs.insert({ psoType, Pso });
		}

		inline void RemovePSO(Charalotte::E_PSOTYPE psoType)
		{
			PSOs.erase(psoType);
		}

		Charalotte::PSO& GetPSOReference(Charalotte::E_PSOTYPE psoType, bool& IsGetSuccess)
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
		Charalotte::PSO& GetPSOReference(Charalotte::E_PSOTYPE psoType)
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

		Charalotte::PSO GetPSOCopy(Charalotte::E_PSOTYPE psoType)
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
		std::unordered_map<Charalotte::E_PSOTYPE, Charalotte::PSO>& GetPSOs()
		{
			return PSOs;
		}
	private:
		std::unordered_map<Charalotte::E_PSOTYPE, Charalotte::PSO> PSOs;
		Charalotte::PSO EmptyPSO;
	};
}

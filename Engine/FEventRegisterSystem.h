#pragma once
#include "Singleton.h"
#include <vector>
#include <functional>
#include <unordered_map>

namespace Charalotte
{
	class FEventRegisterSystem : public Singleton<FEventRegisterSystem>
	{
	public:
		void RegisterFunctionWithInt(std::string ExecuterName, std::function<void(const std::vector<int>&)> func)
		{
			EventWithInt.insert({ ExecuterName, std::move(func) });
		}

		void ExecuteFunctionWithInt(std::string ExecuterName, const std::vector<int>& Params)
		{
			const auto& Iter = EventWithInt.find(ExecuterName);
			if (Iter != EventWithInt.end())
			{
				Iter->second(Params);
			}
		}
	private:
		std::unordered_map<std::string, std::function<void(const std::vector<int>&)>> EventWithInt;
	};
}
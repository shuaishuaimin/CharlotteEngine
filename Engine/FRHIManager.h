#pragma once
#include "RHI.h"
#include "Singleton.h"

namespace Charalotte
{
	class FRHIManager : public Singleton<FRHIManager>
	{
	public:
		FRHIManager();
		~FRHIManager();

		RHI* GetRHIPtr();
	private:
		std::unique_ptr<RHI> RHIIns;
	};
}

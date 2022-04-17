#pragma once
#include <memory>
#include "DDefines.h"
#include "FDevice.h"
#include "SEResourceElement.h"

namespace Charalotte
{
	class FPCRenderTarget 
	{
	public:
		FPCRenderTarget();
		virtual ~FPCRenderTarget(){}
		void Init(FHeapManager* HMgr);
		void SetRTResource(std::shared_ptr<FResource> RTResource)
		{
			mRtResource = RTResource;
		}
		void SetDSResource(std::shared_ptr<FResource> DSResource)
		{
			mDsvResource = DSResource;
		}
		void OnResize(UINT newWidth, UINT newHeight);
		UINT getWight()
		{
			return mWidth;
		}
		UINT GetHeight()
		{
			return mHeight;
		}

		FResource* GetRtResource()
		{
			return mRtResource.get();
		}

		FResource* GetDsResource()
		{
			return mDsvResource.get();
		}

	private:
		std::shared_ptr<FResource> mRtResource = nullptr;
		std::shared_ptr<FResource> mDsvResource = nullptr;

		UINT mWidth = 0;
		UINT mHeight = 0;

	};
}

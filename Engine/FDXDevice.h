#pragma once
#include "FDevice.h"
#include "d3dx12.h"

class FDXDevice : public FDevice {
public:
	FDXDevice(ID3D12Device* DevicePtr){
		Device = DevicePtr;
	}
	virtual ~FDXDevice()
	{}

	ID3D12Device* GetDevice()
	{
		return GetDevice();
	}
private:
	ID3D12Device* Device;
};
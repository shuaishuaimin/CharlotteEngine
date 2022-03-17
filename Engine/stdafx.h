#pragma once
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

#include "FRender.h"
#include "Math.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/quaternion.hpp"
#include "BaseStructAllPlatform.h"
#include "FMathHelper.h"
#include "Singleton.h"
#include "FGameTimer.h"


#if PLATFORM_WINDOWS
#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include "d3dx12.h"
#include "DDSTextureLoader.h"
#include "DXPrimitives.h"
#include "WinMaterialData.h"
#include "FDataProcessor.h"
#include "FCamera.h"
#include "FScene.h"
#endif
	
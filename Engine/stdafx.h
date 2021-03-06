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

#include "FRenderer.h"
#include "Math.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/quaternion.hpp"
#include "STransformDef.h"
#include "FMathHelper.h"
#include "Singleton.h"
#include "FGameTimer.h"
#include "FMaterial.h"
#include "RHI.h"
#include "FRenderer.h"
#include "DDefines.h"

#ifdef RENDER_PLATFORM_DX12
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
#include "DX12RHI.h"
#endif
	
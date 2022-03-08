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
#include <thread>
#include <chrono>
#include <mutex>
#include "FApp.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/quaternion.hpp"
#include "FMathHelper.h"
#include "Singleton.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include "d3dx12.h"
#include "DDSTextureLoader.h"
#include "BaseStructAllPlatform.h"
#include "FWinsApp.h"
#endif
	
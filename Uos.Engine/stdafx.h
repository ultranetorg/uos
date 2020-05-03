#pragma once

//#define DIRECT3D_VERSION		0x0900
//#define DIRECTINPUT_VERSION		0x0800
#define VK_USE_PLATFORM_WIN32_KHR

#include <manipulations.h>
#include <ocidl.h>

#include "Uos/Include.h"

#include <shellscalingapi.h>
#include <dxgi.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
//#include <wincodec.h>




//#include <Vulkan/vulkan/vulkan.h>

#include "Resources/resource.h"

#ifdef UOS_ENGINE_EXPORT_DLL
	#define UOS_ENGINE_LINKING __declspec(dllexport)

	#include <DevIL/include/IL/config.h>
	#include <DevIL/include/IL/ilu.h>

	#include <DirectXTex/DirectXTex.h>
#endif

#ifdef UOS_ENGINE_IMPORT_DLL
	#define UOS_ENGINE_LINKING __declspec(dllimport)
#endif

#define  UOS_ENGINE_DIRECT
//#define  UOS_ENGINE_VULKAN

#include "dxerr.h"

#define Verify(r) CEngineLevel::VerifyCritical(__FUNCTIONW__, __LINE__, r)


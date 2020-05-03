#pragma once
#include "targetver.h"

#include "Uos.Engine/Include.h"

#ifdef UOS_WORLD_EXPORT_DLL
	#define UOS_WORLD_LINKING __declspec(dllexport)
#endif

#include "Globals.h"
#include "resource.h"
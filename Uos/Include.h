#pragma once

#include "stdafx.h"

#define UOS_LINKING __declspec(dllimport)

// Levels
#include "Core.h"
#include "Nexus.h"
#include "EmptyLicenseService.h"
#include "IExecutorProtocol.h"
#include "NativeWindow.h"

// Math
#include "Float4.h"
#include "Plane.h"
#include "Animated.h"
#include "Triangle.h"
#include "BBox.h"
#include "Animation.h"
#include "Size.h"
#include "Float6.h"
#include "Circle.h"
#include "Layout.h"
#include "Polygon.h"

// I/O
#include "ZipStream.h"
#include "AsyncFileStream.h"
#include "MemoryStream.h"
#include "IWriter.h"
#include "WritePool.h"
#include "NativeDirectory.h"
#include "Zip.h"
#include "TextReader.h"

// Util
#include "Meta.h"
#include "RefList.h"
#include "DateTime.h"

#include "WebInformer.h"
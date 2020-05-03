#pragma once
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#ifdef _DEBUG
	#define D3D_DEBUG_INFO

	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include <windows.h>
#include <Windowsx.h>
#include <typeinfo>
#include <clocale>
#include <locale>
#include <process.h>
#include <dbghelp.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <codecvt>
#include <regex>
#include <Strsafe.h>
#include <iomanip> 
#include <wininet.h>
#include <Lmcons.h>
#include <comdef.h>

#include <list>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <locale>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <sys/types.h>
#include <sys/timeb.h>

#include <Zip/zip.h>
#include <Zip/unzip.h>

#include <Ipfs/client.h>

#include <Keccak/Keccak.h>

#include <curl/curl.h>

#include <DirectXMath.h>
#include <d2d1.h>
#include <d2d1helper.h>

#ifdef UOS_EXPORT_DLL
	#define UOS_LINKING __declspec(dllexport)
#endif

#ifdef UOS_EXPORT_LIB
	#define UOS_LINKING
#endif

#ifdef UOS_IMPORT_DLL
	#define UOS_LINKING __declspec(dllimport)
#endif

#ifdef UOS_IMPORT_LIB
	#define UOS_LINKING
#endif


#include "Globals.h"

#pragma warning(disable : 4251) //class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) //non – DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'

//#include "String.h" // #include "List.h" + #include "Array.h"
//const std::wstring::size_type std::wstring::npos = (std::wstring::size_type) -1;


#define  BOOST_ALL_DYN_LINK
#include <boost/algorithm/string.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace uos
{
	typedef boost::multiprecision::uint256_t			uint256;
}

#include "TreeMap.h"
/*

#ifdef _DEBUG
	#define D3D_DEBUG_INFO

	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif*/
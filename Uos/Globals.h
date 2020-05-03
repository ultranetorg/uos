#pragma once

namespace uos
{
	#undef GetClassName

	#define null										NULL
	#define HERE										__FUNCTIONW__,__LINE__

	typedef unsigned int								uint;

	#define UOS_PROJECT_DEVELOPMENT_STAGE				L"Alpha"

	#define UOS_OBJECT_PROTOCOL							L"ocp"

	#define UOS_PROJECT_TARGET_PLATFORM_WIN32_X86		L"Win32.x86"
	#define UOS_PROJECT_TARGET_PLATFORM_WIN32_X64		L"Win32.x64"

	#define UOS_PROJECT_CONFIGURATION_DEBUG				L"Debug"
	#define UOS_PROJECT_CONFIGURATION_RELEASE			L"Release"

	#define UOS_MOUNT_LOCAL								L"Local"
	#define UOS_MOUNT_PERSONAL							L"Personal"
	#define UOS_MOUNT_SERVER							L"Server"
	#define UOS_MOUNT_SERVER_TMP						L"Server.Tmp"
	#define UOS_MOUNT_USER_LOCAL						L"User.Local"
	#define UOS_MOUNT_USER_GLOBAL						L"User.Global"

	#define UO_NAME										L"Ultranet Organization"
	#define UO_NAMESPACE								L"UO"
	//#define UO_PREFIX									L"Ultranet"
	#define UO_COPYRIGHT								L"© 2019 Ultranet Organization"
	#define UO_WEB_COMMUNITY							L"http://forum.ultranet.org/"
	#define UO_WEB_HOME									L"https://ultranet.org"
	#define UO_WEB_SERVICE_ACTIVITY						L"http://ultranet.org/service/activity"


	#define Bit(a, n)									(((a >> n) & 0x1)==1)
		
	#define cleandelete(p)								if(p != null)\
														{\
															if(*((unsigned int *)p) == 0xfeeefeee)\
															{\
																throw CException(HERE, L"p==0xfeeefeee"); \
															}\
															delete p;\
															p=null;\
														}

	#define UOS_RTTI									static CString & GetClassName()\
														{\
															static CString name;\
															if(name.empty())\
															{\
																auto p = (wchar_t *)(__FUNCTIONW__ + wcslen(__FUNCTIONW__));\
																auto n = 0;\
																wchar_t * e = null;\
																wchar_t * b = null;\
																while(n < 3)\
																{\
																	if(*p == L':')\
																	{	\
																		n++;\
																		if(n == 2)\
																			e = p;\
																		if(n == 3)\
																		{\
																			b = p+1;\
																			if(*b == 'C')\
																				b++;\
																			break;\
																		}\
																	}\
																	p--;\
																}\
																name.assign(b, e-b);\
															}\
															return name;\
														}\
														\
														virtual CString & GetInstanceName() override\
														{\
															static CString b;\
															if(b.empty())\
															{\
																auto p = typeid(*this).name();\
																const char * name = strstr(p, "C") + 1; if(!name) name = p;\
																auto n = MultiByteToWideChar(CP_ACP, 0, name, -1, null, 0);\
																b.resize(n - 1); /* ending \0 not needed */\
																MultiByteToWideChar(CP_ACP, 0, name, -1, (LPWSTR)b.data(), (int)b.size());\
															}\
															return b;\
														}\

}

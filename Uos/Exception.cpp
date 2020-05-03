#include "StdAfx.h"
#include "Exception.h"

using namespace uos;

CCodePosition::CCodePosition()
{
}

CCodePosition::CCodePosition(const wchar_t * method, int line)
{
	ClassMethod = method;
	Line = line;
}

CException::CException()
{
}

CException::CException(const wchar_t * m, int l, std::wstring const & msg)
{
	Source	= CCodePosition(m, l);
	Message	= msg;

	if(IsDebuggerPresent())
	{
		//DebugBreak();
	}
}

CException::~CException()
{
}

CLastErrorException::CLastErrorException(const wchar_t * m, int l, int err, const wchar_t * f, ...)
{
	if(wcscmp(UOS_PROJECT_CONFIGURATION, UOS_PROJECT_CONFIGURATION_DEBUG)==0 && IsDebuggerPresent())
	{
		DebugBreak();
	}

	wchar_t s[32768];
	wsprintf(s, L"\n(Last error: %d)", err);

	wchar_t msg[32768];
	va_list marker;
	va_start(marker, f);
	vswprintf_s(msg, _countof(msg), f, marker);
	va_end(marker);

	Source	= CCodePosition(m, l);
	Message	= std::wstring(msg) + s;
}

CAttentionException::CAttentionException(const wchar_t * m, int l, const wchar_t * f, ...)
{
	va_list marker;
	va_start(marker, f);
	wchar_t msg[32768];
	vswprintf_s(msg, _countof(msg), f, marker);
	va_end(marker);

	Source	= CCodePosition(m, l);
	Message	= msg;
}

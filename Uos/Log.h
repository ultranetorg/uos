#pragma once
#include "Timer.h"
#include "NativePath.h"
#include "Event.h"
#include "Timings.h"
#include "IType.h"

namespace uos
{
	class CSupervisor;

	enum class ELogSeverity
	{
		Message, Warning, Error, Debug
	};

	class CLogMessage
	{
		public:
			CString										Text;
			ELogSeverity								ReportStatus;
			
			CLogMessage(const CString & t, ELogSeverity rs)
			{
				Text			= t;
				ReportStatus	= rs;
			}
	};

	class UOS_LINKING CLog : public IType
	{
		public:
			CEvent<CLog *, CLogMessage &>				MessageReceived;
			CList<CLogMessage>							Messages;
			CList<CLogMessage>							Pending;

			CSupervisor	*								Supervisor;
			CString										Name;
			static CString								MessageFormat;
			HANDLE										LogFile = null;
			CLog *										MainLog = null;

			std::mutex									Lock;

			UOS_RTTI		
			CLog(CSupervisor * sv, const CString & name);
			~CLog();

			CString &									GetName();
			void										StartWriting(const CString & directory);
			void										StopWriting();
			void										Commit();
			void										Report(ELogSeverity status, IType * who, const wchar_t * str);

			template<typename ... Args> void ReportMessage(IType * who, const std::wstring& format, Args const & ... args)
			{
				Report(ELogSeverity::Message, who, format, args ...);
			}

			template<typename ... Args> void ReportWarning(IType * who, const std::wstring& format, Args const & ... args)
			{
				Report(ELogSeverity::Warning, who, format, args ...);
			}

			template<typename ... Args> void ReportError(IType * who, const std::wstring& format, Args const & ... args)
			{
				Report(ELogSeverity::Error, who, format, args ...);
			}

			void ReportException(IType * who, CException & e)
			{
				Report(ELogSeverity::Error, who, L"%s:%d - %s", e.Source.ClassMethod, e.Source.Line, e.Message);
			}

			template<typename ... Args> void ReportDebug(IType * who, const std::wstring& format, Args const & ... args)
			{
				Report(ELogSeverity::Debug, who, format, args ...);
			}

			#pragma warning(push)
			#pragma warning(disable: 4996)
			template<typename ... Args> void Report(ELogSeverity s, IType * who, const std::wstring& format, Args const & ... args)
			{
				const auto fmt = format.c_str();
				const size_t size = _snwprintf(nullptr, 0, fmt, CString::ProcessArg(args) ...) + 1;
				
				wchar_t * buf = (wchar_t *)_alloca(size * sizeof(wchar_t));

				_snwprintf(buf, size, fmt, CString::ProcessArg(args) ...);

				Report(s, who,  buf);
			}
			#pragma warning(pop)
	};
}
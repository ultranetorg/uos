#pragma once
#include "Log.h"
#include "Diagnostic.h"

namespace uos
{
	class UOS_LINKING CSupervisor : public IType
	{
		public:
			void										StartWriting(const CString & directory);
			void										StopWriting();
			CString 									GetPathToDataFolder(const CString & path);

			CLog *										CreateLog(const CString & name);
			CDiagnostic *								CreateDiagnostics(const CString & name);
			CLog *										GetLog(const CString & name);
			CDiagnostic *								GetDiagnostics(const CString & name);
			void										WriteDiagnosticFile(const CString & name, CString const & content);
			
			void										Commit();

			CString										Directory;
			CList<CLog *>								Logs;
			CList<CDiagnostic *>						Diagnosticss;
			CLog *										MainLog = null;
			uint64_t									Cycles = 0;

			CEvent<CLog *>								ErrorReported;
			CEvent<CLog *>								LogCreated;
			CEvent<CDiagnostic *>						DiagnosticsCreated;

			UOS_RTTI
			CSupervisor();
			~CSupervisor();

	};
}

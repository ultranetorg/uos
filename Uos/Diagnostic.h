#pragma once
#include "Converter.h"
#include "Array.h"
#include "DiagGrid.h"
#include "Event.h"
#include "IType.h"

namespace uos
{
	struct CDiagElement
	{
		CString Text;
		DWORD	Color;

		CDiagElement(){}
		CDiagElement(const CString & t, DWORD c) : Text(t), Color(c){}
	};

	class CDiagnosticUpdate
	{
		public:
			int Offset;
			int Count;
	};

	class UOS_LINKING CDiagnostic : public IType
	{
		public:
			CString &									GetName();
			void										Clear();
			//void										Add(const wchar_t *, ...);

			template<typename ... Args> void Add(const std::wstring& format, Args const & ... args)
			{
				Lines++;
				Added(CString::Format(format, (args)...), RGB(0, 192, 0));
			}

			void										Add(CDiagnosticUpdate & u, CDiagGrid & g);
			void										Append(const CString & t, DWORD c = RGB(0, 204, 0));
			int											Size();
			bool										ShouldProceed(CDiagnosticUpdate & u, int s);
			bool										ShouldFill(CDiagnosticUpdate & u, int s);

			CEvent<CDiagnosticUpdate &>					Updating;

			CEvent<CString &, DWORD>						Added;
			CEvent<CString &, DWORD>						Appended;

			UOS_RTTI		
			CDiagnostic(const CString & name);
			~CDiagnostic();

			CString										Name;
			//CArray<CArray<CDiagElement>>				Lines;
			int											Lines = 0;
	};
}
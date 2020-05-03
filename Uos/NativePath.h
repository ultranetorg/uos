#pragma once
#include "String.h"

namespace uos
{
	class UOS_LINKING CNativePath
	{
		public:
			bool	static 								IsDirectory(const CString & a);
			bool	static 								IsFile(const CString & a);
			CString	static								Join(const CString & a, const CString & b);
			CString	static								Join(const CString & a, const CString & b, CString const & c);
			CString	static 								GetFileName(const CString & a);
			CString static								GetFileNameBase(const CString & a);
			CString static								Canonicalize(const CString & a);
			CString	static 								GetFileExtension(const CString & a);
			CString	static 								GetDirectoryPath(const CString & a);
			CString	static								GetDirectoryName(const CString & a);
			CString	static 								ReplaceFileName(const CString & path, const CString & filename);
			bool	static 								IsRelative(const CString & s);
			CString	static 								GetSafe(const CString & s);
			bool	static								IsRoot(const CString & a);
			bool	static								IsUNCServer(const CString & a);
			bool	static								IsUNCServerShare (const CString & a);
			CString	static								FindOnPath(const CString & file);

			CString static								AddBackslash(const CString & a);
			CString static								RemoveBackslash(const CString & path);
			
			static CString								EscapeRegex(CString &regex);
			static bool									MatchWildcards(const CString &text, CString wildcardPattern, bool caseSensitive = false);

			static CString GetTmp();

		private:
			CNativePath();
			~CNativePath();
	};
}
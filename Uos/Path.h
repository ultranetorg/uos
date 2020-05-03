#pragma once
#include "Uxx.h"

namespace uos
{
	class UOS_LINKING CPath
	{
		public:
			static CString								Join(CString const & a, CString const & b);
			static CString								ReplaceLast(CString & p, CString const & path);
			static CString								ReplaceExtension(CString const & p, CString const & ext);
			static CString								ReplaceMount(CString & p, CString const & mount);
			static CString								GetName(CString const & n);
			static CString								GetNameBase(CString const & u);
			static CString								GetDirectoryPath(const CString & addr);
			static CString								GetExtension(CString & f);

			static CString								Universalize(CString const & path);
			static CString								Nativize(CString const & path);

	};

}
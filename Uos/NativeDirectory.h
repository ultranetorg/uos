#pragma once
#include "IType.h"
#include "FileStream.h"
#include "Array.h"
#include "IWriterProgress.h"
#include "Guid.h"

namespace uos
{
	enum EDirectoryFlag
	{
		AsPath = 1, SkipServiceElements = 2, SkipHidden = 4, DirectoriesOnly = 8, FilesOnly = 16
	};

	struct CDirectoryEntry
	{
		CString		Path;
		int			Attrs;
	};

	class UOS_LINKING CNativeDirectory
	{
		public:
			static void									Delete(CString const & src, bool premove = true);
			static void									Copy(CString const & src, CString const & dst);
			static void									Create(CString const & src);
			static void									CreateAll(const CString & path, bool tryUntilSuccess);
			static void									Clear(CString const & src);
			static bool									Exists(CString const & l);
			static CArray<CDirectoryEntry>				Find(CString const & folder, const CString & mask, EDirectoryFlag f);

	};
}
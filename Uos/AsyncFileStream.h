#pragma once
#include "FileStream.h"


namespace uos
{
	class CAsyncFileStream : public CFileStream
	{
		public:
		CLevel2 *									Level;
		CThread *									Thread;
		CBuffer										Buffer;

		CAsyncFileStream(CLevel2 * l, const CString & filepath, EFileMode mode) : CFileStream(filepath, mode)
		{
			Level = l;
		}

		~CAsyncFileStream()
		{
		}

		void ReadAsync(std::function<void()> ondone)
		{
			Level->Core->RunThread(Path,[this]()
										{
											Buffer = Read();
										},
										[ondone]()
										{ 
											ondone();
										});
		}
	};
}


#include "StdAfx.h"
#include "Zip.h"
#include "Converter.h"

using namespace uos;

CZipStream::CZipStream(CZipDirectory * zip, const CString & entrypath)
{
	Zip			= zip;
	EntryPath	= entrypath;

	if(zip->GetMode() == EFileMode::Open)
	{
		if(unzLocateFile(zip->GetHandle(), entrypath.ToAnsi().data(), 0) == UNZ_OK)
		{
			if(unzOpenCurrentFile(zip->GetHandle()) == UNZ_OK)
			{
				// WORKAROUND:
				// we read all data at once to allow multiple zip read streams opened
				// try low-level zip api to avoid that

				unz_file_info zInfo; 
				
				if(unzGetCurrentFileInfo(Zip->GetHandle(), &zInfo, 0, 0, 0, 0, 0, 0) != UNZ_OK)
				{
					throw CException(HERE, L"unzGetCurrentFileInfo");
				}

				CBuffer b(zInfo.uncompressed_size);
				if(unzReadCurrentFile(Zip->GetHandle(), b.GetData(), (unsigned int)b.GetSize()) >= 0)
				{
					UnzStream.Write(b.GetData(), b.GetSize());
				}
				else
					throw CException(HERE, L"unzReadCurrentFile");
				
				unzCloseCurrentFile(Zip->GetHandle());
			}
			else
			{
				throw CException(HERE, CString::Format(L"Entry %s couldn`t be opened in %s", entrypath, Zip->GetUri()));
			}
		}
		else
		{
			throw CException(HERE, CString::Format(L"Entry %s not found in %s", entrypath, Zip->GetUri()));
		}
	}
	else if(zip->GetMode() == EFileMode::New)
	{
		tm		when;
		time_t	now;

		time(&now);
		localtime_s(&when, &now);

		zip_fileinfo zipfi;
		ZeroMemory(&zipfi, sizeof(zip_fileinfo));
		zipfi.tmz_date.tm_sec	= when.tm_sec;
		zipfi.tmz_date.tm_min	= when.tm_min;
		zipfi.tmz_date.tm_hour	= when.tm_hour;
		zipfi.tmz_date.tm_mday	= when.tm_mday;
		zipfi.tmz_date.tm_mon	= when.tm_mon;
		zipfi.tmz_date.tm_year	= when.tm_year;

		//int crc = crc32(0, (const Bytef *)data, size);
		//zipOpenNewFileInZip3(Zip, c.WsToS(filepath).c_str(), &zipfi, null, 0, null, 0, null, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0, 15, 8, Z_DEFAULT_STRATEGY, null, crc);
		
		if(zipOpenNewFileInZip(zip->GetHandle(), entrypath.ToAnsi().data(), &zipfi, null, 0, null, 0, null, Z_DEFLATED, Z_DEFAULT_COMPRESSION) == Z_OK)
		{
		}
		else
		{
			throw CException(HERE, CString::Format(L"Entry %s couldn`t be created in %s", entrypath, Zip->GetUri()));
		}
	}
}

CZipStream::~CZipStream()
{
	if(Zip->GetMode() == EFileMode::New)
	{
		zipCloseFileInZip(Zip->GetHandle());
	}
}

int64_t CZipStream::GetSize()
{
	//unz_file_info zInfo;
	//
	//if(unzGetCurrentFileInfo(Zip->GetHandle(), &zInfo, 0, 0, 0, 0, 0, 0) == UNZ_OK)
	//{
	//	return zInfo.uncompressed_size;
	//}
	//return -1;

	return UnzStream.GetSize();
}

bool CZipStream::IsEnd()
{
	return UnzStream.IsEnd();
}

CString CZipStream::GetUri()
{
	return CNativePath::Join(Zip->GetUri(), EntryPath);
}

void CZipStream::Seek(int64_t offset)
{
	throw CException(HERE, L"Not supported");
}

int64_t CZipStream::Read(void * buffer, int64_t size)
{
	assert(size <= UINT_MAX);

	return UnzStream.Read(buffer, size);
	//return unzReadCurrentFile(Zip->GetHandle(), buffer, (unsigned int)size);
}

int64_t CZipStream::Write(const void * data, int64_t size)
{
	if(data != null && size > 0)
	{
		assert(size <= UINT_MAX);

		auto e = zipWriteInFileInZip(Zip->GetHandle(), data, (unsigned int)size);
		if(e != ZIP_OK)
		{
			throw CException(HERE, L"!= ZIP_OK");
		}

		return size;
	}
	else
	{
		throw CException(HERE, L"data == null || size == 0");
	}
}

int64_t CZipStream::Write(CStream * src)
{
	CBuffer b = src->Read();
	return Write(b.GetData(), b.GetSize());
}

int64_t CZipStream::Write(CStream * src, IWriterProgress * p)
{
	CBuffer b = src->Read();
	return Write(b.GetData(), b.GetSize(), p);
}

int64_t CZipStream::Write(const void * data, int64_t size, IWriterProgress * p)
{
	assert(size <= UINT_MAX);
	
	BYTE * pointer = (BYTE *)data;
	unsigned int bsize;
	unsigned int rsize;

	rsize = (unsigned int)size;
	bsize = (unsigned int)(size < 1024 ? size : 1024);

	while(rsize > 0)
	{
		zipWriteInFileInZip(Zip->GetHandle(), pointer, bsize);
		pointer += bsize;
		rsize	-= bsize;

		p->ReportProgress(100 * ((unsigned int)size - rsize) / (unsigned int)size);

		if(0 < rsize && rsize < 1024)
		{
			bsize = rsize;
		}
	} 

	return size - rsize;
}	

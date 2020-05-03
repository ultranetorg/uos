#include "StdAfx.h"
#include "Zip.h"
#include "Converter.h"

using namespace uos;

voidpf ZCALLBACK fopen_mem_func OF((voidpf opaque, const char* filename, int mode));
uLong ZCALLBACK fread_mem_func OF((voidpf opaque, voidpf stream, void* buf, uLong size));
uLong ZCALLBACK fwrite_mem_func OF((voidpf opaque, voidpf stream, const void* buf, uLong size));
long ZCALLBACK ftell_mem_func OF((voidpf opaque, voidpf stream));
long ZCALLBACK fseek_mem_func OF((voidpf opaque, voidpf stream, uLong offset, int origin));
int ZCALLBACK fclose_mem_func OF((voidpf opaque, voidpf stream));
int ZCALLBACK ferror_mem_func OF((voidpf opaque, voidpf stream));

typedef struct ourmemory_s {
	char *base; /* Base of the region of memory we're using */
	uLong size; /* Size of the region of memory we're using */
	uLong limit; /* Furthest we've written */
	uLong cur_offset; /* Current offset in the area */
} ourmemory_t;

voidpf ZCALLBACK fopen_mem_func(voidpf opaque, const char* filename, int mode)
{
	ourmemory_t * mem = (ourmemory_t *)malloc(sizeof(*mem));
	if(mem == NULL)
		return NULL; /* Can't allocate space, so failed */

					 /* Filenames are specified in the form :
					 *    <hex base of zip file>+<hex size of zip file>
					 * This may not work where memory addresses are longer than the
					 * size of an int and therefore may need addressing for 64bit
					 * architectures
					 */
	int64_t s;

	if(sscanf_s(filename, "%p+%llx", &mem->base, &s) != 2)
		return NULL;

	mem->size = (uLong)s;

	if(mode & ZLIB_FILEFUNC_MODE_CREATE)
		mem->limit = 0; /* When writing we start with 0 bytes written */
	else
		mem->limit = mem->size;

	mem->cur_offset = 0;

	return mem;
}

uLong ZCALLBACK fread_mem_func(voidpf opaque, voidpf stream, void* buf, uLong size)
{
	ourmemory_t *mem = (ourmemory_t *)stream;

	if(size > mem->size - mem->cur_offset)
		size = mem->size - mem->cur_offset;

	memcpy(buf, mem->base + mem->cur_offset, size);
	mem->cur_offset += size;

	return size;
}

uLong ZCALLBACK fwrite_mem_func(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
	ourmemory_t *mem = (ourmemory_t *)stream;

	if(size > mem->size - mem->cur_offset)
		size = mem->size - mem->cur_offset;

	memcpy(mem->base + mem->cur_offset, buf, size);
	mem->cur_offset += size;
	if(mem->cur_offset > mem->limit)
		mem->limit = mem->cur_offset;

	return size;
}

long ZCALLBACK ftell_mem_func(voidpf opaque, voidpf stream)
{
	ourmemory_t *mem = (ourmemory_t *)stream;

	return mem->cur_offset;
}

long ZCALLBACK fseek_mem_func(voidpf opaque, voidpf stream, uLong offset, int origin)
{
	ourmemory_t *mem = (ourmemory_t *)stream;
	uLong new_pos;
	switch(origin)
	{
		case ZLIB_FILEFUNC_SEEK_CUR:
			new_pos = mem->cur_offset + offset;
			break;
		case ZLIB_FILEFUNC_SEEK_END:
			new_pos = mem->limit + offset;
			break;
		case ZLIB_FILEFUNC_SEEK_SET:
			new_pos = offset;
			break;
		default: return -1;
	}

	if(new_pos > mem->size)
		return 1; /* Failed to seek that far */

	if(new_pos > mem->limit)
		memset(mem->base + mem->limit, 0, new_pos - mem->limit);

	mem->cur_offset = new_pos;
	return 0;
}

int ZCALLBACK fclose_mem_func(voidpf opaque, voidpf stream)
{
	ourmemory_t *mem = (ourmemory_t *)stream;

	/* Note that once we've written to the buffer we don't tell anyone
	about it here. Probably the opaque handle could be used to inform
	some other component of how much data was written.

	This, and other aspects of writing through this interface, has
	not been tested.
	*/

	free(mem);
	return 0;
}

int ZCALLBACK ferror_mem_func(voidpf opaque, voidpf stream)
{
	ourmemory_t *mem = (ourmemory_t *)stream;
	/* We never return errors */
	return 0;
}

void fill_memory_filefunc(zlib_filefunc_def * pzlib_filefunc_def)
{
	pzlib_filefunc_def->zopen_file = fopen_mem_func;
	pzlib_filefunc_def->zread_file = fread_mem_func;
	pzlib_filefunc_def->zwrite_file = fwrite_mem_func;
	pzlib_filefunc_def->ztell_file = ftell_mem_func;
	pzlib_filefunc_def->zseek_file = fseek_mem_func;
	pzlib_filefunc_def->zclose_file = fclose_mem_func;
	pzlib_filefunc_def->zerror_file = ferror_mem_func;
	pzlib_filefunc_def->opaque = NULL;
}

CZipDirectory::CZipDirectory(const CString & zippath, EFileMode mode)
{
	ZipPath	= zippath;
	Mode	= mode;

	if(mode == EFileMode::Open)
	{
		ZFile = unzOpen(zippath.ToAnsi().data());

		if(ZFile == null)
		{
			throw CException(HERE, CString::Format(L"File %s not found", zippath));
		}
	}
	if(mode == EFileMode::New)
	{
		ZFile = zipOpen(zippath.ToAnsi().data(), APPEND_STATUS_CREATE);

		if(ZFile == null)
		{
			throw CException(HERE, CString::Format(L"Can`t create %s", zippath));
		}
	}
}

CZipDirectory::CZipDirectory(CStream * s, EFileMode mode)
{
	Mode	= mode;

	if(mode == EFileMode::Open)
	{
		Buffer = s->Read();

		auto path = CString::Format(L"%p+%llx", Buffer.GetData(), Buffer.GetSize()).ToAnsi();

		zlib_filefunc_def filefunc32 = { 0 };
		fill_memory_filefunc(&filefunc32);
		
		ZFile = unzOpen2(path.data(), &filefunc32);
			 
		if(ZFile == null)
		{
			throw CException(HERE, L"Cant open zip memry stream");
		}
	}
	if(mode == EFileMode::New)
	{
		throw CException(HERE, L"Not implemented");
		///ZFile = zipOpen(CConverter::ToAnsi(zippath).c_str(), APPEND_STATUS_CREATE);
		///
		///if(ZFile == null)
		///{
		///	throw CException(HERE, L"Cann`t create %s", zippath.c_str());
		///}
	}
}

CZipDirectory::~CZipDirectory()
{
	if(Mode == EFileMode::Open)
	{
		unzClose(ZFile);
	}
	if(Mode == EFileMode::New)
	{
		zipClose(ZFile, null);
	}
}

EFileMode CZipDirectory::GetMode()
{
	return Mode;
}

void * CZipDirectory::GetHandle()
{
	return ZFile;
}

CString CZipDirectory::GetUri()
{
	return ZipPath;
}

CString CZipDirectory::GetUri(const CString & entrypath)
{
	return CNativePath::Join(ZipPath, entrypath);
}

CStream * CZipDirectory::OpenWriteStream(const CString & entry)
{
	return new CZipStream(this, entry);
}

CStream * CZipDirectory::OpenReadStream(const CString & entry)
{
	return new CZipStream(this, entry);
}

void CZipDirectory::Close(CStream * s)
{
	delete s;
}

CList<CStorageEntry> uos::CZipDirectory::Enumerate(CString const & mask)
{
	unzFile z = unzOpen(ZipPath.ToAnsi().data());
	if(z == null)
	{
		throw CException(HERE, CString::Format(L"Can`t open %s", ZipPath));
	}

	CList<CStorageEntry> files;

	int e = unzGoToFirstFile(z);

	while(e == UNZ_OK)
	{
		char b[32768];
		unz_file_info i;
		ZeroMemory(&i, sizeof(zip_fileinfo));
		unzGetCurrentFileInfo(z, &i, b, sizeof(b), null, 0, null, 0);

		auto path = CString::FromAnsi(b);

		if(CNativePath::MatchWildcards(path, mask, false))
		{
			CStorageEntry se;
			se.Path = path; 
			se.Type = (path.EndsWith(L"/") ? CDirectory::GetClassName() : CFile::GetClassName());

			files.push_back(se);
		}

		e = unzGoToNextFile(z);
	}
	unzClose(z);
	return files;
}

uos::CList<uos::CFSRegexItem> uos::CZipDirectory::EnumerateByRegex(CString const & mask)
{
	throw CException(HERE, L"Not implemented");
}

void CZipDirectory::Delete()
{
	throw CException(HERE, L"Not supported");
}

int CZipDirectory::GetSize(const CString & filepath)
{
	unzFile z = unzOpen(ZipPath.ToAnsi().data());
	if(z == null)
	{
		throw CException(HERE, CString::Format(L"Cann`t open %s", ZipPath));
	}
	
	unz_file_info	zInfo;

	if(unzLocateFile(z, filepath.ToAnsi().data(), 0) == UNZ_OK)
	{
		if(unzGetCurrentFileInfo(z, &zInfo, 0, 0, 0, 0, 0, 0) == UNZ_OK)
		{
			unzClose(z);
			return zInfo.uncompressed_size;
		}
	}
	throw CException(HERE, CString::Format(L"File %s not found or can`t be accessed in %s", filepath, ZipPath));
}

void CZipDirectory::Add(const CString & dstpath, CStream * src)
{
	CZipStream s(this, dstpath);
	s.Write(src);
}

void CZipDirectory::Add(const CString & dstpath, CStream * src, IWriterProgress * p)
{
	CZipStream s(this, dstpath);
	s.Write(src, p);
}

bool CZipDirectory::Contains(const CString & filepath)
{
	unzFile z = unzOpen(ZipPath.ToAnsi().data());
	if(z == null)
	{
		throw CException(HERE, CString::Format(L"Cann`t open %s", ZipPath));
	}

	bool r = unzLocateFile(z, filepath.ToAnsi().data(), 0) == UNZ_OK;

	unzClose(z);
	
	return r;
}

std::vector<uint8_t> CZip::Compress(void *in_data, size_t in_data_size)
{
	assert(in_data_size <= UINT_MAX);
	
	std::vector<uint8_t> out;

	z_stream zs;                        // z_stream is zlib's control structure
	memset(&zs, 0, sizeof(zs));

	if(deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK)
		throw CException(HERE, L"deflateInit failed");

	zs.next_in = (Bytef*)in_data;
	zs.avail_in = (unsigned int)in_data_size;           // set the z_stream's input

	int ret;
	char outbuffer[32768];


	// retrieve the compressed bytes blockwise
	do{
		zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
		zs.avail_out = sizeof(outbuffer);

		ret = deflate(&zs, Z_FINISH);

		if(out.size() < zs.total_out)
		{
			out.insert(out.end(), outbuffer, outbuffer + zs.total_out - out.size());
		}
	}while(ret == Z_OK);

	deflateEnd(&zs);

	if(ret != Z_STREAM_END)
	{
		throw CException(HERE, L"Compression failed");
	}


	return out;
}

CArray<char> CZip::Decompress(void *in_data, size_t in_data_size)
{
	assert(in_data_size <= UINT_MAX);

	z_stream zs;                        // z_stream is zlib's control structure
	memset(&zs, 0, sizeof(zs));

	if (inflateInit(&zs) != Z_OK)
		throw CException(HERE, L"inflateInit failed");

	zs.next_in = (Bytef*)in_data;
	zs.avail_in = (unsigned int)in_data_size;

	int ret;
	char outbuffer[32768];
	CArray<char> out;

	// get the decompressed bytes blockwise using repeated calls to inflate
	do
	{
		zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
		zs.avail_out = sizeof(outbuffer);

		ret = inflate(&zs, 0);

		if(out.size() < zs.total_out)
		{
			out.insert(out.end(), outbuffer, outbuffer + zs.total_out - out.size());
		}

	}while(ret == Z_OK);

	inflateEnd(&zs);

	if(ret != Z_STREAM_END)
	{
		throw CException(HERE, L"Decompression failed");
	}

	return out;
}


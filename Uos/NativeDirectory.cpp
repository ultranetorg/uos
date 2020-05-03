#include "StdAfx.h"
#include "NativeDirectory.h"


using namespace uos;

void CNativeDirectory::Create(CString const & src)
{
	CreateDirectory((L"\\\\?\\" + src).data(), null);
}

void CNativeDirectory::Delete(CString const & src, bool premove) 
{
	WIN32_FIND_DATA ffd;

	DWORD e;
	int n = 0;
	
	auto s = src;

	if(premove)
	{
		auto rnd = s + CGuid::Generate64();
		if(MoveFile((L"\\\\?\\" + s).data(), (L"\\\\?\\" + rnd).data()) == FALSE)
		{
			//throw CException(HERE, L"Unable to pre move: %s" + s);
		}
		
		s = rnd;
	}

	HANDLE h = FindFirstFile((L"\\\\?\\" + s + L"\\*.*").c_str(), &ffd);
	BOOL r = (h != INVALID_HANDLE_VALUE);

	while(r)
	{
		if(wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0)
		{
		}
		else if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{	
			Delete(s + L"\\" + ffd.cFileName, false);
		}
		else
		{
			n = 0;

			while(DeleteFile((L"\\\\?\\" + s + L"\\" + ffd.cFileName).data()) == FALSE)
			{
				Sleep(1);
				e = GetLastError();
				n++;

				if(n > 1000)
				{
					throw CException(HERE, L"Unable to DeleteFile after 1000 attempts");
				}
			}
		}	

		r = FindNextFile(h, &ffd);
	}
	FindClose(h);

	while(RemoveDirectory((L"\\\\?\\" + s).data()) == FALSE)
	{
		Sleep(1);
		e = GetLastError();
		n++;

		if(e == 2 || e == 3) // already removed
			break;

		if(n > 1000)
		{
			throw CException(HERE, L"Unable to RemoveDirectory after 1000 attempts");
		}
	}
}

void CNativeDirectory::Copy(CString const & src, CString const & dst)
{
	WIN32_FIND_DATA ffd;

	auto s = L"\\\\?\\" + src;
	auto d = L"\\\\?\\" + dst;

	DWORD e;
	int n = 0;

	while(CreateDirectory(d.data(), null) == FALSE)
	{
		Sleep(1);
		e = GetLastError();
		n++;

		if(e == 183) // already exists
			break;

		if(n > 1000)
		{
			throw CException(HERE, L"Unable to create directory after 1000 attempts");
		}
	}
	

	HANDLE h = FindFirstFile(CNativePath::Join(s, L"*.*").c_str(), &ffd);
	BOOL r = (h != INVALID_HANDLE_VALUE);

	while(r)
	{
		if(wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0);
		else if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{	
			Copy(src + L"\\" + ffd.cFileName, dst + L"\\" + ffd.cFileName);
		}
		else
		{
			n = 0;

			while(CopyFile((s + L"\\" + ffd.cFileName).data(), (d + L"\\" + ffd.cFileName).data(), FALSE) == FALSE)
			{
				Sleep(1);
				e = GetLastError();
				n++;

				if(n > 1000)
				{
					throw CException(HERE, L"Unable to copy file after 1000 attempts");
				}
			}
		}	

		r = FindNextFile(h, &ffd);
	}
	FindClose(h);
}

CArray<CDirectoryEntry> CNativeDirectory::Find(CString const & dir, const CString & mask, EDirectoryFlag f)
{
	CArray<CDirectoryEntry> files;

	if(dir == L"\\")
	{
		PWSTR ppath;

		IShellItem * m_currentBrowseLocationItem;
		auto hr = ::SHGetKnownFolderItem(FOLDERID_ComputerFolder, static_cast<KNOWN_FOLDER_FLAG>(0), nullptr, IID_PPV_ARGS(&m_currentBrowseLocationItem));

		IShellFolder * searchFolder;
		hr = m_currentBrowseLocationItem->BindToHandler(nullptr, BHID_SFObject, IID_PPV_ARGS(&searchFolder));

		IEnumIDList * fileList;

		if(S_OK == searchFolder->EnumObjects(nullptr, SHCONTF_FOLDERS|SHCONTF_FASTITEMS, &fileList))
		{
			ITEMID_CHILD * idList = nullptr;
			unsigned long fetched;

			while(S_OK == fileList->Next(1, &idList, &fetched))
			{

				IShellItem2 * shellItem;
				hr = SHCreateItemWithParent(nullptr, searchFolder, idList, IID_PPV_ARGS(&shellItem));

				if(SUCCEEDED(hr))
				{
					hr = shellItem->GetDisplayName(SIGDN_FILESYSPATH, &ppath);

					if(ppath)
					{
						files.push_back(CDirectoryEntry{f & AsPath?dir + ppath:ppath, FILE_ATTRIBUTE_DIRECTORY});

						CoTaskMemFree(ppath);
					}
				}

				ILFree(idList);
			}
		}
	} 
	else
	{
		WIN32_FIND_DATA ffd;

		HANDLE h = FindFirstFile(CNativePath::Join(L"\\\\?\\" + dir, mask).c_str(), &ffd);
		BOOL r = (h != INVALID_HANDLE_VALUE);

		while(r)
		{
			if((f & SkipServiceElements) && (wcscmp(ffd.cFileName, L".")== 0 || wcscmp(ffd.cFileName, L"..")== 0));
			else if((f & SkipHidden)		&& ((ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0));
			else if((f & FilesOnly)			&& ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0));
			else if((f & DirectoriesOnly)	&& ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0));
			else
				files.push_back(CDirectoryEntry{f & AsPath ? dir + ffd.cFileName : ffd.cFileName, int(ffd.dwFileAttributes)});
			r = FindNextFile(h, &ffd);
		}
		FindClose(h);
	}

	return files;
}
		
bool CNativeDirectory::Exists(CString const & l)
{
	DWORD dwAttrib = GetFileAttributes((L"\\\\?\\" + l).data());

	return l == L"\\" || (dwAttrib != INVALID_FILE_ATTRIBUTES) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);	
}

void CNativeDirectory::CreateAll(const CString & path, bool tryUntilSuccess)
{
	auto parts = path.Split(L"\\");

	CString s;
	for(auto & i : parts)
	{
		s += i + L"\\";

		if(CNativePath::IsRoot(s) || CNativePath::IsUNCServer(s) || CNativePath::IsUNCServerShare(s))
		{
			continue;
		}

		if(!Exists(s))
		{
			Create(s);
		}
	}
}


void CNativeDirectory::Clear(CString const & src)
{
	throw CException(HERE, L"Not tested");

	WIN32_FIND_DATA ffd;

	HANDLE h = FindFirstFile(CNativePath::Join(L"\\\\?\\" + src, L"\\*.*").c_str(), &ffd);
	BOOL r = (h != INVALID_HANDLE_VALUE);

	while(r)
	{
		if(wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0)
			;
		else if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{	
			Delete(src + L"\\" + ffd.cFileName);
		}
		else
		{
			DeleteFile((src + L"\\" + ffd.cFileName).data());
		}	

		r = FindNextFile(h, &ffd);
	}
	FindClose(h);
}
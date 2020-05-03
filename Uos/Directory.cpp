#include "stdafx.h"
#include "Directory.h"
//#include "Nexus.h"
#include "Storage.h"

using namespace uos;

CDirectoryServer::CDirectoryServer(CServer * s, CString const & path, CString const & npath) : CDirectory(s, CDirectory::GetClassName() + L"-" + path)
{
	Path = npath;
}

CDirectoryServer::~CDirectoryServer()
{

}

CList<CStorageEntry> CDirectoryServer::Enumerate(CString const & mask)
{
	CList<CStorageEntry> ooo;

	auto ini = CNativePath::IsFile(CNativePath::Join(Path, L"desktop.ini"));

	for(auto i : CNativeDirectory::Find(Path, mask, EDirectoryFlag(SkipHidden|SkipServiceElements)))
	{
		CStorageEntry e;

		if(ini && i.Path.EndsWith(L".lnk"))
		{
			wchar_t b[1024];
			GetPrivateProfileString(L"LocalizedFileNames", CNativePath::GetFileName(i.Path).data(), null, b, _countof(b), CNativePath::Join(Path, L"desktop.ini").data());

			if(wcslen(b) > 0)
			{
				auto a = CString(b).Split(L",");

				ExpandEnvironmentStrings(a[0].data()+1, b, _countof(b));

				auto h = LoadLibraryEx(b, null, LOAD_LIBRARY_AS_IMAGE_RESOURCE);

				if(!h)
				{
					h = LoadLibraryEx(CString(b).Replace(L"Program Files (x86)", L"Program Files").data(), null, LOAD_LIBRARY_AS_IMAGE_RESOURCE); // stupid workaround
				}
						
				if(h)
				{
					int n = LoadString(h, abs(CInt32::Parse(a[1])), b, _countof(b));

					if(n > 0)
					{
						e.NameOverride = b;
					}

					FreeLibrary(h);
				}
			}
		}

		//if(i.Path.EndsWith(L":\\"))
		//{
		//	i.Path.resize(2);
		//}

		e.Path = CPath::Join(Url.GetId(), CPath::Universalize(i.Path));
		e.Type = i.Attrs & FILE_ATTRIBUTE_DIRECTORY ? CDirectory::GetClassName() : CFile::GetClassName();

		ooo.push_back(e);
	}

	return ooo;
}

void CDirectoryServer::Delete()
{
	CNativeDirectory::Delete(Path);
}

CStream * CDirectoryServer::OpenWriteStream(CString const & name)
{
	return dynamic_cast<CStorage *>(Server)->OpenWriteStream(CPath::Join(CUol::GetObjectID(Url.Object), name));
}

CStream * CDirectoryServer::OpenReadStream(CString const & name)
{
	return dynamic_cast<CStorage *>(Server)->OpenReadStream(CPath::Join(CUol::GetObjectID(Url.Object), name));
}

void CDirectoryServer::Close(CStream * s)
{
	return dynamic_cast<CStorage *>(Server)->Close(s);
}

CList<CFSRegexItem> CDirectoryServer::EnumerateByRegex(CString const & pattern)
{
	CList<CFSRegexItem> o;

	std::wregex r(pattern);

	std::wsregex_iterator end;

	for(auto i : Enumerate(L"*.*"))
	{
		auto name = CPath::GetNameBase(i.Path); 

		auto ri = std::wsregex_iterator(name.begin(), name.end(), r);

		if(ri != end)
		{
			CFSRegexItem fi;
			fi.Path = i.Path;

			for(auto mi : *ri)
			{
				fi.Matches.push_back(mi.str());
			}
			
			o.push_back(fi);
		}

		//Unid = (*mri)[1].str();
	}

	return o;
}

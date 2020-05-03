#include "StdAfx.h"
#include "Storage.h"
#include "Nexus.h"

using namespace uos;

CStorage::CStorage(CLevel2 * l, CServerInfo * info) : CServer(l, info), CDirectoryServer(this, L"/", L"")
{
	Level = l;

	Protocols[UOS_FILE_SYSTEM_PROTOCOL] = null;
}

CStorage::~CStorage()
{
	while(auto i = Objects.Find([](auto j){ return j->Shared;  }))
	{
		DestroyObject(i);
	}
}

IProtocol * CStorage::Connect(CString const & pr)
{
	if(pr == UOS_FILE_SYSTEM_PROTOCOL)
	{
		return this;
	}

	return null;
}

void CStorage::Disconnect(IProtocol * p)
{
	if(p == this)
	{
		//for(auto i : Streams)
		//{
		//	Destroy(i);
		//}

		for(auto i : Directories)
		{
			DestroyObject(i);
		}

		WriteStreams.clear();
		ReadStreams.clear();
		Directories.clear();

	}
}

void CStorage::Start(EStartMode sm)
{
	//CNativeDirectory::Create(Level->Core->MapToDatabase(UOS_MOUNT_GLOBAL));
	//CNativeDirectory::Create(Level->Core->MapToDatabase(UOS_MOUNT_SYSTEM_GLOBAL));
	//CNativeDirectory::Create(Level->Core->MapToDatabase(UOS_MOUNT_SYSTEM_LOCAL));
	//CNativeDirectory::Create(Level->Core->TmpFolder);
}

void CStorage::CreateMounts(CServerInfo * s)
{
	CNativeDirectory::Create(Level->Core->MapToDatabase(CNativePath::Join(UOS_MOUNT_USER_GLOBAL, s->Url.Server)));
	CNativeDirectory::Create(Level->Core->MapToDatabase(CNativePath::Join(UOS_MOUNT_USER_LOCAL, s->Url.Server)));
}

CString CStorage::UniversalToNative(CString const & p)
{
	auto path = p;
	auto mount = path.Substring(L'/', 1);
	auto lpath = p.size() > mount.size() + 1 ? path.Substring(path.find(L'/', path.find(L'/') + 1) + 1) : L"";

	if(mount == UOS_MOUNT_LOCAL)
	{
		if(lpath.empty())
			return L"\\";
		if(lpath.size() == 2 && lpath[1] == L':')
			return lpath + L"\\";
		else
			path = CPath::Nativize(lpath);
	}
	//else if(mount == UOS_MOUNT_PERSONAL)
	//{
	//	path = Level->Core->MapToGlobal(CPath::Nativize(lpath));
	//}
	else if(mount == UOS_MOUNT_SERVER)
	{
		path = Level->Core->GetPathTo(ESystemPath::Servers, CPath::Nativize(lpath));
	}
	else if(mount == UOS_MOUNT_SERVER_TMP)
	{
		path = Level->Core->MapToTmp(CPath::Nativize(lpath));
	}
	else if(mount == UOS_MOUNT_USER_LOCAL)
	{
		path = Level->Core->MapToDatabase(CPath::Nativize(path));
	}
	else if(mount == UOS_MOUNT_USER_GLOBAL)
	{
		path = Level->Core->MapToDatabase(CPath::Nativize(path));
	}
	
	return path;	
}

CString CStorage::NativeToUniversal(CString const & path)
{
	return L"/" UOS_MOUNT_LOCAL L"/" + CPath::Universalize(path);
}

CObject<CDirectory> CStorage::OpenDirectory(CString const & addr)
{
	auto d = Directories.Find([addr](auto i){ return i->Url.GetId() == addr; });

	if(d)
	{
		d->Take();
		return d;
	}

	auto mount = addr.Substring(L'/', 1);
	auto native = UniversalToNative(addr);
	
	if(CNativeDirectory::Exists(native))
	{
		d = new CDirectoryServer(this, addr, native);

		Directories.push_back(d);
		RegisterObject(d, false);
		d->Free();
		return d;
	}
	if(addr == L"/")
	{
		RegisterObject(this, false);
		return this;
	}
	
	return null;
}

CStream * CStorage::OpenWriteStream(CString const & addr)
{
	auto dir = CPath::GetDirectoryPath(addr);

	if(!Exists(dir))
	{
		CreateAllDirectories(dir);
	}
	
	CString path = addr;

	CFileStream * s = null;

	//if(path.StartsWith(UOS_MOUNT_LOCAL) || path.StartsWith(UOS_MOUNT_GLOBAL) || path.StartsWith(UOS_MOUNT_SYSTEM) || path.StartsWith(UOS_MOUNT_SYSTEM_LOCAL) || path.StartsWith(UOS_MOUNT_SYSTEM_GLOBAL))
	{
		path = UniversalToNative(addr);
		s = new CFileStream(path, EFileMode::New);
		WriteStreams.push_back(s);
	}

	return s;
}

CStream * CStorage::OpenReadStream(CString const & u)
{
	CFileStream * s = null;

	auto n = Resolve(u);

	try
	{
		s = new CFileStream(n, EFileMode::Open);
	}
	catch(CFileException & )
	{
	}

	if(s)
	{
		ReadStreams.push_back(s);
	}

	return s;
}


bool CStorage::CanRemap(CString const & addr)
{
	return addr.StartsWith(L"/" UOS_MOUNT_SERVER L"/");
}

CString CStorage::RemapToCommon(CString const & path)
{
	auto s = UniversalToNative(L"/" UOS_MOUNT_SERVER L"/");
	auto p = path.Substring(s.size());
	return CNativePath::Canonicalize(Level->Core->GetPathTo(ESystemPath::Common, p));
}

CAsyncFileStream * CStorage::OpenAsyncReadStream(CString const & addr)
{
	auto path = UniversalToNative(addr);
	auto s = new CAsyncFileStream(Level, path, EFileMode::Open);
	ReadStreams.push_back(s);

	return s;
}

void CStorage::Close(CStream * f)
{
	if(WriteStreams.Contains(f))
		WriteStreams.Remove(f);

	if(ReadStreams.Contains(f))
		ReadStreams.Remove(f);
	
	delete f;
}

void CStorage::Close(CDirectory * d)
{
	if(d == this)
	{
		DeleteObject(d);
		return;
	}

	if(d->GetRefs() == 1)
	{
		Directories.Remove(d->As<CDirectoryServer>());
		DeleteObject(d);
	}
	else
		d->Free();
}

CString CStorage::MapPath(CString const & mount, CString const & path)
{
	return CPath::Join(L"/" + mount, path);
}

CString CStorage::MapPath(CUsl & u, CString const & path)
{
	return MapPath(UOS_MOUNT_SERVER, CPath::Join(u.Server, path));
}

bool CStorage::Exists(CString const & u)
{
	auto  n = Resolve(u);

	return (CNativePath::IsDirectory(n) || CNativePath::IsFile(n));
}

CString CStorage::Resolve(CString const & u)
{
	auto n = UniversalToNative(u);

	if(CNativePath::IsDirectory(n) || CNativePath::IsFile(n))
	{
		return n;
	}
	else
	{
		if(CanRemap(u))
		{
			auto r = RemapToCommon(n);
			
			if(CNativePath::IsDirectory(r) || CNativePath::IsFile(r))
			{
				return r;
			}
		}
	}

	return n;
}

void CStorage::CreateDirectory(CString const & o)
{
	auto l = UniversalToNative(o);

	if(!CNativeDirectory::Exists(l))
	{
		CNativeDirectory::Create(l);
	}
}

void CStorage::CreateAllDirectories(CString const & o)
{
	CNativeDirectory::CreateAll(UniversalToNative(o), true);
}
// 
// void CStorage::CreateGlobalDirectory(CString const & path)
// {
// 	auto d = MapGlobalPath(path);
// 	CreateDirectory(d);
// }

void CStorage::CreateGlobalDirectory(CNexusObject * o, CString const & path)
{
	auto d = o->MapGlobalPath(path);
	CreateDirectory(d);
}

void CStorage::CreateGlobalDirectory(CServer * s, CString const & path)
{
	auto d = s->MapUserGlobalPath(path);
	CreateDirectory(d);
}

void CStorage::CreateLocalDirectory(CString const & path)
{
	auto d = MapLocalPath(path);
	CreateDirectory(d);
}

void CStorage::CreateLocalDirectory(CNexusObject * o, CString const & path)
{
	auto d = o->MapLocalPath(path);
	CreateDirectory(d);
}

void CStorage::CreateLocalDirectory(CServer * s, CString const & path)
{
	auto d = s->MapUserLocalPath(path);
	CreateDirectory(d);
}

CList<CStorageEntry> CStorage::Enumerate(CString const & mask)
{
	CList<CStorageEntry> ooo;

	//ooo.push_back(CStorageEntry(MapPath(UOS_MOUNT_PERSONAL, L""),		CDirectory::GetClassName()	));
	ooo.push_back(CStorageEntry(MapPath(UOS_MOUNT_LOCAL, L""),			CDirectory::GetClassName()	));
	ooo.push_back(CStorageEntry(MapPath(UOS_MOUNT_SERVER, L""),			CDirectory::GetClassName()	));
	ooo.push_back(CStorageEntry(MapPath(UOS_MOUNT_USER_GLOBAL, L""),	CDirectory::GetClassName()	));
	ooo.push_back(CStorageEntry(MapPath(UOS_MOUNT_USER_LOCAL, L""),		CDirectory::GetClassName()	));

	return ooo;
}

void CStorage::DeleteFile(CString const & f)
{
	auto path = UniversalToNative(f);

	CLocalFile::Delete(path);
}

void CStorage::DeleteDirectory(CString const & f)
{
	auto path = UniversalToNative(f);

	CNativeDirectory::Delete(path);
}

CList<CStorageEntry> CStorage::Enumerate(CString const & dir, CString const & mask)
{
	auto d = OpenDirectory(dir);
	
	if(d)
	{
		auto & o = d->Enumerate(mask);
		Close(d);
		return o;
	}
	else
		return {};
}

CUol CStorage::ToUol(CString const & type, CString const & path)
{
	return CUol(CServer::Url, type + L"-" + path);
}

CString CStorage::GetType(CString const & path)
{
	auto p = UniversalToNative(path);

	if(CNativePath::IsDirectory(p))
	{
		return CDirectory::GetClassName();
	}
	if(CNativePath::IsFile(p))
	{
		return CFile::GetClassName();
	}
	else
		throw CException(HERE, L"Unknown type");
}
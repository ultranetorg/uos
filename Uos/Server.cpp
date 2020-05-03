#include "StdAfx.h"
#include "Server.h"
#include "Nexus.h"

using namespace uos;

CServer::CServer(CLevel2 * l, CServerInfo * info) : Level(l)
{
	Url = info->Url;
	Info = info;

}
	
CServer::~CServer()
{
}

void CServer::RegisterObject(CNexusObject * o, bool shared)
{
	if(Objects.Has([o](auto i){ return i->Url.Object == o->Url.Object; }))
	{	
		throw CException(HERE, L"Already registered");
	}
	
	o->Take();

	o->Shared = shared;
	Objects.push_back(o);
}

CNexusObject * CServer::CreateObject(CString const & name)
{
	return null;
}

CNexusObject * CServer::FindObject(CUol const & u)
{
	if(!u.IsEmpty() && ((CUsl)u != Url))
	{
		throw CException(HERE, L"Alien system object");
	}
	return FindObject(u.Object);
}

CNexusObject * CServer::FindObject(CString const & name)
{
	auto o = Objects.Find([&name](auto i){ return i->Url.Object == name; });

	if(!o && !name.empty() && Exists(name))
	{
		o = CreateObject(name);

		LoadObject(o);

		RegisterObject(o, o->Shared);
		o->Free();
	}

	return o;
}

bool CServer::Exists(CString const & name)
{
	auto f = MapUserGlobalPath(name + L".object");
	auto g = MapUserGlobalPath(name);
	auto l = MapUserLocalPath(name);

	return Level->Nexus->Storage->Exists(f) || Level->Nexus->Storage->Exists(g) || Level->Nexus->Storage->Exists(l);
}

void CServer::LoadObject(CNexusObject * o)
{
	auto f = MapUserGlobalPath(o->Url.Object + L".object");
	auto g = MapUserGlobalPath(o->Url.Object);
	auto l = MapUserLocalPath(o->Url.Object);

	if(Level->Nexus->Storage->Exists(f))
	{
		o->Shared = true;
				
		auto s = Level->Nexus->Storage->OpenReadStream(f);
		o->LoadInfo(s);
		Level->Nexus->Storage->Close(s);
	}

	if(Level->Nexus->Storage->Exists(g) || Level->Nexus->Storage->Exists(l))
	{
		o->Load();
	}
}

void CServer::DeleteObject(CNexusObject * r)
{
	auto name = r->Url.Object;
	auto shared = r->Shared;

	DestroyObject(r, false);

	if(shared)
	{
		if(CUol::GetObjectID(name).Has([](auto c){ return !iswalnum(c); }))
		{
			throw CException(HERE, L"Incorrect permanent object name");
		}

		Level->Nexus->Storage->DeleteFile(Level->Nexus->Storage->MapPath(UOS_MOUNT_USER_GLOBAL, name + L".object"));
		Level->Nexus->Storage->DeleteFile(Level->Nexus->Storage->MapPath(UOS_MOUNT_USER_LOCAL, name + L".object"));
	}

	//r->Free();
}

void CServer::DestroyObject(CNexusObject * o, bool save)
{
	if(Objects.Contains(o))
		Objects.Remove(o);
	else
		return;

	if(save && o->Shared)
	{
		auto s = Level->Nexus->Storage->OpenWriteStream(MapUserGlobalPath(o->Url.Object + L".object"));
		o->SaveInfo(s);
		Level->Nexus->Storage->Close(s);
	}

	o->Destroying(o);
	o->Free();
}

CTonDocument * CServer::LoadServerDocument(CString const & path)
{
	auto storage = Level->Nexus->Storage;

	if(auto s = storage->OpenReadStream(MapPath(path)))
	{
		auto d = new CTonDocument(CXonTextReader(s));
		storage->Close(s);
		return d;
	}
	else
		return null;
}

CTonDocument * CServer::LoadGlobalDocument(CString const & path)
{
	auto storage = Level->Nexus->Storage;
		
	if(auto s = storage->OpenReadStream(MapUserGlobalPath(path)))
	{
		auto d = new CTonDocument(CXonTextReader(s));
		storage->Close(s);
		return d;
	}
	else
		return null;
}

CString CServer::MapRelative(CString const & path)
{
	return CPath::Join(Url.Server, path);
}

CString CServer::MapUserLocalPath(CString const & path)
{
	return Level->Nexus->Storage->MapPath(UOS_MOUNT_USER_LOCAL, MapRelative(path));
}

CString CServer::MapUserGlobalPath(CString const & path)
{
	return Level->Nexus->Storage->MapPath(UOS_MOUNT_USER_GLOBAL, MapRelative(path));
}

CString CServer::MapTmpPath(CString const & path)
{
	return Level->Nexus->Storage->MapPath(UOS_MOUNT_SERVER_TMP, MapRelative(path));
}

CString CServer::MapPath(CString const & path)
{
	return Level->Nexus->Storage->MapPath(UOS_MOUNT_SERVER, MapRelative(path));
}

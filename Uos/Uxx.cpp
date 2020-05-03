#include "stdafx.h"
#include "Uxx.h"

using namespace uos;

const CString		CUsl::Protocol = UOS_OBJECT_PROTOCOL;
const std::wstring	CUsl::TypeName = L"usl";
const std::wstring	CUol::TypeName = L"uol";

CUsl::CUsl()
{
}

CUsl::CUsl(CString const & u) : CUsl(CUrl(u)) 
{
}

CUsl::CUsl(const CUrl & u)
{
	Domain = u.Domain;
	Server = u.Path.Substring(L"/", 0);
}

CUsl::CUsl(CString const & d, CString const & s)
{
	Domain = d;
	Server = s;
}

CString CUsl::ToString() const
{
	CString e;

	if(Server.empty() && Domain.empty())
	{
		return e;
	}

	if(Server.empty())
	{
		throw CException(HERE, L"Server can not be empty");
	}

	e += Protocol + L"://" + Domain + L"/" + Server;

	return e;

	//return Protocol + L"://" + Domain + L"/" + Hub + L"?origin=" + Origin;
}

bool CUsl::operator != (const CUsl & u) const
{
	return !const_cast<CUsl &>(*this).Equals(u);
	//return Domain != u.Domain || Hub != u.Hub;
}

bool CUsl::operator == (const CUsl & u) const
{
	return const_cast<CUsl &>(*this).Equals(u);
	//return Domain == u.Domain && Hub == u.Hub;
}

bool CUsl::IsUsl(const CUrl & u)
{
	return u.Protocol == Protocol && !u.Path.empty();
}

CUsl::operator CUrl() const
{
	CUrl u;
	u.Protocol = Protocol;
	u.Domain = Domain;
	u.Path = Server;

	return u;
}

std::wstring CUsl::GetTypeName()
{
	return TypeName;
}

void CUsl::Read(CStream * s)
{
	CString t;
	t.Read(s);

	CUrl::Read(t, null, &Domain, &Server, null);

	Server = Server.Substring(L'/', 0);
}

int64_t CUsl::Write(CStream * s)  
{
	return ToString().Write(s);
}

void CUsl::Write(std::wstring & s)
{
	s += ToString();
}

void CUsl::Read(const std::wstring & addr)
{
	CUrl::Read(addr, null, &Domain, &Server, null);

	Server = Server.Substring(L'/', 0);
}

ISerializable * CUsl::Clone()
{
	return new CUsl(*this);
}

bool CUsl::Equals(const ISerializable & a) const
{
	return Domain == static_cast<CUsl const &>(a).Domain && Server == static_cast<CUsl const &>(a).Server;
	//return CUsl::operator==(static_cast<const CUsl&>(a));
}

bool CUsl::IsEmpty()
{
	return Domain.empty() && Server.empty();
}

//////////////////////// Uol /////////////////////////////////////

CUol::CUol(CString const & d, CString const & s, CString const & o)
{
	Domain = d;
	Server = s;
	Object = o;
}

CUol::CUol()
{
}

CUol::CUol(const CUrl & u)
{
	Domain = u.Domain;
	Server = u.Path.Substring(L"/", 0);
	Object = u.Path.Substring(u.Path.find(L"/") + 1);
	Parameters = u.Query;
}

CUol::CUol(CUrl & u, CString const & o) : CUol(u)
{
	Object = o;
}

CUol::CUol(CUsl & u, CString const & o)
{
	Domain = u.Domain;
	Server = u.Server;
	Object = o;
}

CUol::CUol(CUsl & u, CString const & o, CMap<CString, CString> const & params) : CUol(u, o)
{
	Parameters = params;
}

bool CUol::operator==(const CUol & a) const
{
	return Equals(a);
}

bool CUol::operator!=(const CUol & a) const
{
	return !Equals(a);
}

CUol & CUol::operator= (CUrl & addr)
{
	auto u = const_cast<CUrl &>(addr);

	Domain		= u.Domain;
	Server		= u.Path.Substring(L"/", 0);
	Object		= u.Path.Substring(u.Path.find(L"/") + 1);
	Parameters	= u.Query;

	return *this;
}

bool CUol::IsEmpty() const
{
	return Domain.empty() && Server.empty() && Object.empty() && Parameters.empty();// && Params.empty();
}

CString CUol::GetType()
{
	return Object.substr(0, Object.find(L'-'));
}

CString CUol::GetId() const
{
	return Object.substr(Object.find(L'-') + 1);
}

CUol::operator CUrl() const
{
	CUrl u;
	u.Protocol = Protocol;
	u.Domain = Domain;
	u.Path = Server + L"/" + Object;
	u.Query = Parameters;

	return u;
}

CString CUol::ToString() const
{
	return !IsEmpty() ? __super::ToString() + L"/" + Object + (Parameters.empty() ? L"" : L"?" + CString::Join(Parameters, [](auto & i){ return i.first + L"=" + i.second; }, L"&")) : L"";
}

std::wstring CUol::GetTypeName()
{
	return TypeName;
}

void CUol::Read(const std::wstring & addr)
{
	CString p;
	CUrl::Read(addr, null, &Domain, &p, &Parameters);
	
	Server = p.Substring(L"/", 0);
	Object = p.Substring(p.find(L"/") + 1);
}

ISerializable * CUol::Clone()
{
	return new CUol(*this);
}

bool CUol::Equals(const ISerializable & a) const
{
	if(!__super::Equals(a))
	{
		return false;
	}
	if(Object != ((CUol const &)a).Object)
	{
		return false;
	}

	auto & ap = ((CUol &)a).Parameters.begin();
	auto & bp = Parameters.begin();

	while(ap != ((CUol &)a).Parameters.end() && bp != Parameters.end())
	{
		if(ap->second != bp->second)
		{
			return false;
		}

		ap++;
		bp++;
	}


	return true;
//	return CUol::operator==(static_cast<const CUol&>(a));
}

CString CUol::GetObjectType(CString const & o)
{
	return o.Substring(0, o.find('-'));
}

CString CUol::GetObjectID(CString const & o)
{
	return o.Substring(o.find('-') + 1);
}

bool CUol::IsValid(const CUrl & u)
{
	return CUsl::IsUsl(u) && !u.Path.Substring(L"/", 1).empty();
}

//////////////////////// Uor /////////////////////////////////////

CUrq::CUrq(CString const & u) : CUrl(u)
{
}

CUrq::CUrq(CUrl const & u) : CUrl(u)
{
}

CUrq::CUrq(CUol const & u)
{
	Protocol = u.Protocol;
	Domain = u.Domain;
	Path = u.Server + L"/" + u.Object;
}

CString CUrq::GetSystem() const
{
	return Path.Substring(L'/', 0);
}

CString CUrq::GetObject() const
{
	auto i = Path.find(L'/');

	if(i != CString::npos)
		return Path.Substring(i + 1);
	else
		return L"";

}

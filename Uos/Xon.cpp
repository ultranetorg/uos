#include "StdAfx.h"
#include "Xon.h"

using namespace uos;

const std::wstring CXon::TypeName = L"node";

CXon::CXon(const CString & name)
{
	Name = name;
}

CXon::CXon(CXon && d)
{
	Children = d.Children;
	Name = d.Name;
	Value = d.Value;

	d.Value = null;
	d.Children.clear();
	d.Removed.clear();
	d.Templates.clear();
}

CXon::CXon(const CXon & xon)
{
	throw CException(HERE, L"Not implemented");
}

CXon::~CXon()
{
	Clear();
}

void CXon::Clear()
{
	delete Value;
	Value = null;

	for(auto i : Children)
	{
		delete i;
	}
	for(auto i : Removed)
	{
		delete i;
	}
	if(IsTemplatesOwner)
	{
		for(auto i : Templates)
		{
			delete i;
		}
	}

	Children.clear();
	Removed.clear();
	Templates.clear();
}

bool CXon::IsAncestor(CXon * ancestor)
{
	CXon * a = Parent;
	while(a != null)
	{
		if(a == ancestor)
		{
			return true;
		}
		a = a->Parent;
	}
		
	return false;
}

CXon * CXon::One(const CString & name)
{
	auto nodes = name.Split(L"/");

	auto i = nodes.begin();
		
	//auto p = One(*i);
	auto p = Children.Find([i](auto j){ return j->Name == *i; });

	if(p != null)
	{
		i++;
	
		for(;i != nodes.end(); i++)
		{
			p = p->Children.Find([i](auto j){ return j->Name == *i; });
			if(p == null)
			{
				return null;
			}
		}
	}
	return p;	 
}

CArray<CXon *> CXon::Many(const CString & name)
{
	CArray<CXon *> o;

	auto p = this;
	auto i = name.find_last_of('/');
	auto last = name;

	if(i != std::string::npos)
	{
		auto path = name.substr(0, i);
		last = name.substr(i+1);
		p = One(path);
	}

	for(auto i : p->Children)
	{
		if(i->Name == last)
		{
			o.push_back(i);
		}
	}

	return o;
}

CArray<CXon *> CXon::ManyOf(const CString & name)
{
	CArray<CXon *> o;

	auto p = this;
	auto i = name.find_last_of('/');
	auto last = name;

	if(i != std::string::npos)
	{
		auto parent = name.substr(0, i);
		last = name.substr(i+1);
		p = One(parent);
	}

	for(auto i : p->Children)
	{
		if(i->Id == last)
		{
			o.push_back(i);
		}
	}

	return o;
}
/*

CArray<CParameter *> CParameter::GetDefaultParameters(const CString & name)
{
	CArray<CParameter *> o;

	auto p = this;
	auto i = name.find_last_of('/');
	auto last = name;
		
	if(i != std::string::npos)
	{
		auto path = name.substr(0, i);
		last = name.substr(i+1);
		p = dynamic_cast<CParameter *>(GetParameter(path));
	}

	for(auto i : p->Children)
	{
		if(i->Name == last && (i->State == EParameterState::Default || i->State == EParameterState::Removed))
		{
			o.push_back(i);
		}
	}

	return o;
}*/

CXon * CXon::GetOrAdd(const CString & name)
{
	auto p = One(name);
	return p ? p : Add(name);
}

void CXon::Add(CXon * p)
{
	p->Parent = this;
	Children.push_back(p); 
}

CXon * CXon::Add(const CString & name)
{	
	auto definition = Templates.Find([this, name](CXon * i){ return i->Name == name; });
	auto n = definition ? definition->CloneInternal(this) : new CXon(name);
	n->Parent = this;
	Children.push_back(n); 
	return n; 
}
	
void CXon::Remove(CXon * p)
{
	Children.Remove(p);

	auto t = Templates.Find([p](CXon * i){ return i->Name == p->Name; });
	if(t)
	{
		Removed.push_back(p);
	} 
	else
	{
		delete p;
	}
}

CXon * CXon::GetRoot()
{
	auto p = this;
	while(p->Parent)
	{
		p = p->Parent;
	}
	return p;
}

void CXon::GiveValue()
{
	if(!Value)
	{
		//delete Value;
		auto p = Parent;
		while(p && !p->Value)
		{
			p = p->Parent;
		}

		Value = p->Value->Clone();
	}
}

void CXon::Set(const ISerializable & v)
{
	if(!Value)
	{
		GiveValue();
	}
	//Type = ((ISerializable &)v).GetTypeName();
	dynamic_cast<CXonSimpleValue *>(Value)->Set(v);
}

void CXon::Set(bool v					){ Set(CBool(v)); }
void CXon::Set(const wchar_t * v		){ Set((ISerializable &)CString(v)); }
void CXon::Set(const CString &  v		){ Set((ISerializable &)CString(v)); }
void CXon::Set(int v					){ Set(CInt32(v)); }
void CXon::Set(float v					){ Set(CFloat(v)); }
void CXon::Set(CArray<int> &	v		){ Set(CInt32Array(v)); }
void CXon::Set(CArray<float> &	v		){ Set(CFloatArray(v)); }
void CXon::Set(CArray<CFloat2> & v		){ Set(CFloat2Array(v)); }
void CXon::Set(CArray<CFloat3> & v		){ Set(CFloat3Array(v)); }

bool		CXon::AsBool()		{	return Get<CBool>(); }
int			CXon::AsInt32()		{	return Get<CInt32>(); }
float		CXon::AsFloat32()	{	return Get<CFloat>(); }
CString 	CXon::AsString()	{	return Get<CString>(); }

void CXon::OnChildValueAssigned()
{
	//State = EParameterState::Assigned;
	if(Parent != null)
	{
		Parent->OnChildValueAssigned();
	}
}

bool CXon::Equals(const CXon & a)
{
	if(Name != a.Name)
	{
		return false;
	}

	if(((Value == null) != (a.Value == null)) || (Value && a.Value && !(Value->Equals(*a.Value))))
	{
		return false;
	}

	auto & ap = (a).Children;
	auto & bp = Children;

	if(ap.size() == bp.size())
	{
		for(auto i : ap)
		{
			auto j = bp.Findi([i](auto x){ return x->Name == i->Name; });
			if(	j == bp.end() || 
				ap.Count([i](auto x){ return x->Name == i->Name; }) != bp.Count([i](auto x){ return x->Name == i->Name; }) ||
				!i->Equals(**j))
			{
				return false;
			}
		}
	}
	else
		return false;

	return true;
}

bool CXon::Equals(const CXonValue &) const
{
	throw CException(HERE, L"Not implemented");
}

void CXon::Set(CXonValue * v)
{
	delete Value;
	Value = v->Clone();
}
	
CXon * CXon::CloneInternal(CXon * parent)
{
	auto p = new CXon(Name);
	p->Parent = parent;

	for(auto i : Children)
	{
		p->Children.push_back(i->CloneInternal(p));
	}

	p->Templates = Templates;

	if(Value)
	{
		p->Value = Value->Clone();
	}
	return p;
}

CXonValue * CXon::Clone()
{
	//throw CException(HERE, L"Not implemented");
	return CloneInternal(null);
}

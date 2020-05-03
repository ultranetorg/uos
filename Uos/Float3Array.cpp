#include "StdAfx.h"
#include "Float3Array.h"

using namespace uos;

const std::wstring CFloat3Array::TypeName = L"float32.vector3.array";

std::wstring CFloat3Array::GetTypeName()
{
	return TypeName;
}

void CFloat3Array::Read(CStream * s)
{
	auto n = ReadSize(s);
	
	resize((size_t)n);

	for(auto i=0u; i<n; i++)
	{
		(*this)[i].Read(s);
	}
}

int64_t CFloat3Array::Write(CStream * s)  
{
	auto ss = WriteSize(s, size());

	for(auto & i : *this)
	{
		ss += i.Write(s);
	}
	
	return ss;
}

void CFloat3Array::Write(std::wstring & s)
{
	wchar_t b[128];

	for(auto & i : *this)
	{
		StringCbPrintf(b, sizeof(b), L"%g %g %g", i.x, i.y, i.z);
		s += b;
		if(&i != &back())
			s += L" ";
	}
}

void CFloat3Array::Read(const std::wstring & v)
{
	if(v.empty())
	{
		return;
	}

	CString s = v;
	boost::trim(s);
	auto p = s.Split(L" ");

	if(p.size() % 3 != 0)
	{
		throw CException(HERE, L"Wrong text data");
	}

	resize(p.size()/3);
	for(auto i = 0U; i < p.size()/3; i++)
	{
		(*this)[i].x = CFloat::Parse(p[i*3 + 0]);
		(*this)[i].y = CFloat::Parse(p[i*3 + 1]);
		(*this)[i].z = CFloat::Parse(p[i*3 + 2]);
	}
}

ISerializable * CFloat3Array::Clone()
{
	return new CFloat3Array(*this);
}

bool CFloat3Array::Equals(const ISerializable & a) const
{
	return (size() == ((CFloat3Array &)a).size()) && memcmp(&front(), &((CFloat3Array &)a).front(), size() * sizeof(int)) == 0;
}
/*
void CVector3ArrayParameter::Load(CTextNode * cvalues)
{
	auto parts = cvalues->Value.Split(L",");
	Value.reserve(parts.size()/3);
	for(auto i = parts.begin(); i != parts.end(); i+=2)
	{
		Value.push_back(CFloat3(CFloat::Parse(*i), CFloat::Parse(*(i+1)), CFloat::Parse(*(i+2))));
	}
}

void CVector3ArrayParameter::Load(CStream * values, int size)
{
	if(size > 0 && size % sizeof(CFloat3) != 0) 
	{
		throw CException(HERE, L"Wrong size of data");
	}

	Value.resize(size/sizeof(CFloat3));
	values->Read(Value.data(), size);
}

void CVector3ArrayParameter::SetValue(CArray<CFloat3> & v)
{
	if(v != Value)
	{
		OnValuePreAssigned();
		Value = v;
		OnValueAssigned();
	}
}

CArray<CFloat3> & CVector3ArrayParameter::GetValue()
{
	return Value;
}

CArray<CFloat3> & CVector3ArrayParameter::GetDefault()
{
	auto static v = CArray<CFloat3>();	
	return v;
}

void CVector3ArrayParameter::Serialize(CString & m)
{
	m.assign(CString::Join(GetValue(), [](CFloat3 & i){ return CConverter::ToString(i.x) + L"," + CConverter::ToString(i.y) + L"," + CConverter::ToString(i.z); }, L","));
}

void CVector3ArrayParameter::Serialize(CStream * s)
{
	s->Write(GetValue().data(), GetBinarySerializationSize());
}

int CVector3ArrayParameter::GetBinarySerializationSize()
{
	return GetValue().size() * sizeof(CFloat3);
}

bool CVector3ArrayParameter::Validate(const CString & v)
{
	return true;
}

CParameter * CVector3ArrayParameter::Clone(CParameter * parent, EParameterState::Type state)
{
	auto p = dynamic_cast<CVector3ArrayParameter *>(CParameter::Clone(parent, state)); 
	p->Value = Value;
	return p;
}

void CVector3ArrayParameter::Commit()
{
	CParameter::Commit();
}*/	

#include "StdAfx.h"
#include "Float2Array.h"

using namespace uos;

const std::wstring CFloat2Array::TypeName = L"float32.vector2.array";

std::wstring CFloat2Array::GetTypeName()
{
	return TypeName;
}

void CFloat2Array::Read(CStream * s)
{
	auto n = ReadSize(s);
	
	resize((size_t)n);

	for(auto i=0u; i<n; i++)
	{
		(*this)[i].Read(s);
	}
}

int64_t CFloat2Array::Write(CStream * s)  
{
	auto ss = WriteSize(s, size());

	for(auto & i : *this)
	{
		ss += i.Write(s);
	}
	
	return ss;
}

void CFloat2Array::Write(std::wstring & s)
{
	wchar_t b[128];

	for(auto & i : *this)
	{
		StringCbPrintf(b, sizeof(b), L"%g %g", i.x, i.y);
		s += b;
		if(&i != &back())
			s += L" ";
	}
}

void CFloat2Array::Read(const std::wstring & v)
{
	CString s = v;
	boost::trim(s);
	auto p = s.Split(L" ");

	if(p.size() < 2 || p.size() % 2 != 0)
	{
		throw CException(HERE, L"Wrong text data");
	}

	resize(p.size()/2);
	for(auto i = 0U; i < p.size()/2; i++)
	{
		(*this)[i].x = CFloat::Parse(p[i*2 + 0]);
		(*this)[i].y = CFloat::Parse(p[i*2 + 1]);
	}
}

ISerializable * CFloat2Array::Clone()
{
	return new CFloat2Array(*this);
}

bool CFloat2Array::Equals(const ISerializable & a) const
{
	return (size() == ((CFloat2Array &)a).size()) && memcmp(&front(), &((CFloat2Array &)a).front(), size() * sizeof(int)) == 0;
}


/*
void CVector2ArrayParameter::Load(CTextNode * cvalues)
{
	auto parts = cvalues->Value.Split(L",");
	Value.reserve(parts.size()/2);
	for(auto i = parts.begin(); i != parts.end(); i+=2)
	{
		Value.push_back(CFloat2(CFloat::Parse(*i), CFloat::Parse(*(i+1))));
	}
}

void CVector2ArrayParameter::Load(CStream * cvalues, int csize)
{
	if(csize > 0 && csize % sizeof(CFloat2) != 0)
	{
		throw CException(HERE, L"Wrong size of data");
	}

	Value.resize((int)csize/sizeof(CFloat2));
	cvalues->Read(Value.data(), csize);
}

void CVector2ArrayParameter::SetValue(CArray<CFloat2> & v)
{
	if(v != Value)
	{
		OnValuePreAssigned();
		Value = v;
		OnValueAssigned();
	}

}

CArray<CFloat2> & CVector2ArrayParameter::GetValue()
{
	return Value;
}

CArray<CFloat2> & CVector2ArrayParameter::GetDefault()
{
	auto static v = CArray<CFloat2>();	
	return v;
}

void CVector2ArrayParameter::Serialize(CString & m)
{
	m.assign(CString::Join(GetValue(), [](CFloat2 & i){ return CConverter::ToString(i.x) + L"," + CConverter::ToString(i.y); }, L","));
}

void CVector2ArrayParameter::Serialize(CStream * s)
{
	s->Write(GetValue().data(), GetBinarySerializationSize());
}

int CVector2ArrayParameter::GetBinarySerializationSize()
{
	return GetValue().size() * sizeof(CFloat2);
}

bool CVector2ArrayParameter::Validate(const CString & v)
{
	return true;
}

CParameter * CVector2ArrayParameter::Clone(CParameter * parent, EParameterState::Type state)
{
	auto p = dynamic_cast<CVector2ArrayParameter *>(CParameter::Clone(parent, state)); 
	p->Value = Value;
	return p;
}

void CVector2ArrayParameter::Commit()
{
	CParameter::Commit();
}*/	

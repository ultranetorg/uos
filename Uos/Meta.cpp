#include "StdAfx.h"
#include "Meta.h"

using namespace uos;

/*
CMeta::CMeta()
{
}

CMeta::~CMeta()
{
}

void CMeta::Add(const CString & key, void * value)
{
	if(Contain(key))
	{
		throw CException(HERE, L"Key already exists: ", key);
	}
	else
	{
		Metas.push_back(CMetaItem(key, value));
	}
}

void * CMeta::Get(const CString & key)
{
	for(auto i : Metas)
	{
		if(i->Key == key)
		{
			return i->Value;
		}
	}
	throw CException(HERE, L"Meta not found: ", key);
}

void CMeta::Remove(const CString & key)
{
	for(auto i : Metas)
	{
		if(i->Key == key)
		{
			Metas.erase(i);
			return;
		}
	}
	throw CException(HERE, L"Meta not found: ", key);
}

bool CMeta::Contain(const CString & key)
{
	for(auto i : Metas)
	{
		if(i->Key == key)
		{
			return true;
		}
	}
	return false;
}*/

/////////////// Item //////////////////

CMetaItem::CMetaItem(const CString & key, const void * value)
{
	Key = key;
	Pointer = const_cast<void *>(value);
}

CMetaItem::CMetaItem(const CString & key, CBuffer & value)
{
	Key = key;
	Buffer = value;
}

CMetaItem::CMetaItem(const CString & key, const CString & value)
{
	Key = key;
	String = value;
}

CMetaItem::CMetaItem(const CString & key, CShared * value)
{
	Key = key;
	Shared = value;
}

CMetaItem::~CMetaItem()
{
}


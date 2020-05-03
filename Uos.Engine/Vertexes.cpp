#include "StdAfx.h"
#include "Vertexes.h"

using namespace uos;

CVertexes::~CVertexes()
{
	if(Type == CFloat::TypeName)
	{
		if(Dim == 2)	delete (CArray<CFloat2> *)Data;
		if(Dim == 3)	delete (CArray<CFloat3> *)Data;

	}
	else if(Type == CInt32::TypeName)
	{
		if(Dim == 1)	delete (CArray<int> *)Data;
	}
}

int CVertexes::GetVertexSize()
{
	if(Type == CFloat::TypeName)	return sizeof(float) * Dim; else
	if(Type == CInt32::TypeName)	return sizeof(int) * Dim;

	throw CException(HERE, L"Type not supported");
}

int CVertexes::GetCount()
{
	if(Type == CFloat::TypeName)
	{
		if(Dim == 2)	return (int)((CArray<CFloat2> *)Data)->size(); else
		if(Dim == 3)	return (int)((CArray<CFloat3> *)Data)->size();

	}
	else if(Type == CInt32::TypeName)
	{
		if(Dim == 1)	return (int)((CArray<int> *)Data)->size();
	}

	throw CException(HERE, L"Type not supported");
}

int CVertexes::GetSize()
{
	return GetVertexSize() * GetCount();
}

void CVertexes::Insert(void * vertexes, int offset /*= -1*/)
{
	if(Type == CFloat::TypeName)
	{
		if(Dim == 2)
		{
			if(!Data)
				Data = new CArray<CFloat2>();

			auto & d = *((CArray<CFloat2> *)Data);
			auto & ed = *((CArray<CFloat2> *)vertexes);
			d.insert(offset == -1 ? d.end() : d.begin() + offset, ed.begin(), ed.end());
			return;
		}
		if(Dim == 3)
		{
			if(!Data)
				Data = new CArray<CFloat3>();

			auto & d = *((CArray<CFloat3> *)Data);
			auto & ed = *((CArray<CFloat3> *)vertexes);
			d.insert(offset == -1 ? d.end() : d.begin() + offset, ed.begin(), ed.end());
			return;
		}
	}
	else if(Type == CInt32::TypeName)
	{
		if(Dim == 1)
		{
			if(!Data)
				Data = new CArray<int>();

			auto & d = *((CArray<int> *)Data);
			auto & ed = *((CArray<int> *)vertexes);
			d.insert(offset == -1 ? d.end() : d.begin() + offset, ed.begin(), ed.end());
			return;
		}
	}

	throw CException(HERE, L"Type not supported");
}

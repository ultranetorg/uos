#include "StdAfx.h"
#include "Mesh.h"
#include "DirectMesh.h"

using namespace uos;

CMesh::CMesh(CEngineLevel * l) : CEngineEntity(l)
{
	Realization = new CDirectMesh(l, this);
}
	
CMesh::~CMesh()
{
	for(auto i : Vertexes)
	{
		delete i;
	}

	delete Realization;
}

void CMesh::Clear()
{
	for(auto i : Vertexes)
	{
		delete i;
	}
	Vertexes.clear();
	Indexes.clear();

	BBox.Max = 0;
	BBox.Min = 0;
}

void CMesh::SetIndexes(CArray<int> & a)
{
	Indexes.assign(a.begin(), a.end());
	Realization->Invalidate(false, false, true);
}

void CMesh::SetPrimitiveInfo(EPrimitiveType pt)
{
	PrimitiveType	= pt;
}

void CMesh::SetFillMode(EPrimitiveFill f)
{
	FillMode = f;
}

bool CMesh::IsReady()
{
	int c = -1;
	for(auto i : Vertexes) // check all have the same count
	{
		if(c == -1)
		{
			c = i->GetCount();
		}
		else if(c != i->GetCount())
		{
			return false;
		}
	}

	return !Vertexes.empty() && !Indexes.empty() && c > 0 &&  PrimitiveType != EPrimitiveType::Null;
}

void CMesh::SetBBox(CAABB bb)
{
	BBox = bb;
}

CAABB & CMesh::GetBBox()
{
	return BBox;
}

void CMesh::Save(CXon * r)
{
	auto pv = r->Add(L"Vertexes");

	for(auto e : Vertexes)
	{
		if(e->Type == CFloat::TypeName)
		{
			if(e->Dim == 2)	pv->Add(e->Name)->Set(*((CArray<CFloat2> *)e->Data));
			if(e->Dim == 3)	pv->Add(e->Name)->Set(*((CArray<CFloat3> *)e->Data));
		}
		else if(e->Type == CInt32::TypeName)
		{
			if(e->Dim == 1)	pv->Add(e->Name)->Set(*((CArray<int> *)e->Data));
		}
	}

	if(!Indexes.empty())
		r->Add(L"Indexes")->Set(Indexes);

	auto pt = r->Add(L"PrimitiveType");
	switch(PrimitiveType)
	{
		case EPrimitiveType::PointList    :	pt->Set(L"PointList"	); break;
		case EPrimitiveType::LineList     :	pt->Set(L"LineList"		); break;
		case EPrimitiveType::LineStrip    :	pt->Set(L"LineStrip"	); break;
		case EPrimitiveType::TriangleList :	pt->Set(L"TriangleList"	); break;
		case EPrimitiveType::TriangleStrip:	pt->Set(L"TriangleStrip"); break;
		case EPrimitiveType::TriangleFan  :	pt->Set(L"TriangleFan"	); break;
	}
}

void CMesh::Load(CXon * r)
{
	auto p = r->One(L"Vertexes");
	for(auto i : p->Children)
	{
		if(i->Id == CFloat2Array::TypeName)	SetVertices(i->Name, i->Get<CFloat2Array>()); else
		if(i->Id == CFloat3Array::TypeName)	SetVertices(i->Name, i->Get<CFloat3Array>()); else
		if(i->Id == CInt32Array::TypeName)	SetVertices(i->Name, i->Get<CInt32Array>());
	}
	
	p = r->One(L"Indexes");
	if(p != null)
	{
		SetIndexes(p->Get<CInt32Array>());
	}
	
	auto pt = r->One(L"PrimitiveType")->AsString();
	if(pt == L"PointList"		)	PrimitiveType = EPrimitiveType::PointList    ; else
	if(pt == L"LineList"		)	PrimitiveType = EPrimitiveType::LineList     ; else
	if(pt == L"LineStrip"		)	PrimitiveType = EPrimitiveType::LineStrip    ; else
	if(pt == L"TriangleList"	)	PrimitiveType = EPrimitiveType::TriangleList ; else
	if(pt == L"TriangleStrip"	)	PrimitiveType = EPrimitiveType::TriangleStrip; else
	if(pt == L"TriangleFan"		)	PrimitiveType = EPrimitiveType::TriangleFan  ;
}

CMeshIntersection CMesh::Intersect(CRay & ray)
{
	CTriIntersection tis;
	CMeshIntersection is;

	auto & positions = GetVertexes<CFloat3>(UOS_MESH_ELEMENT_POSITION);

	if(PrimitiveType == EPrimitiveType::TriangleList)
	{
		for(unsigned int i=0; i<Indexes.size(); i+=3)
		{
			if(CTriangle::Intersect(positions[Indexes[i+0]], positions[Indexes[i+1]], positions[Indexes[i+2]], ray, true, &tis))
			{
				if(tis.Distance < is.Distance)
				{
					is.Distance		= tis.Distance;
					is.Point		= GetIntersectionPoint(tis, i);
					is.StartIndex	= i;				
				}
			}
		}
	}
	else
		throw CException(HERE, L"Not supported");

	return is;
}

CFloat3 CMesh::GetIntersectionPoint(CTriIntersection & is, int start)
{
	auto i = start;

	auto & v = GetVertexes<CFloat3>(UOS_MESH_ELEMENT_POSITION);;

	return v[Indexes[i]] + (v[Indexes[i+1]] - v[Indexes[i]]) * is.U + (v[Indexes[i+2]] - v[Indexes[i]]) * is.V;
}

void CMesh::Merge(CMesh * m)
{
	auto ioffset = Vertexes.empty() ? 0 : Vertexes[0]->GetCount();

	for(auto a : m->Vertexes)
	{
		 auto b = Vertexes.Find([a](auto j){ return a->Name == j->Name; });
		 if(b)
			b->Insert(a->Data);
		 else
			AddVertices(a->Name, a->Type, a->Dim, a->Data);
	}
	///Vertices.Add(m->Vertices);
	///UVs.Add(m->UVs);

	auto ix = m->Indexes;

	for(auto & i : ix)
	{
		i += ioffset;
	}

	Indexes.Add(ix);

	Realization->Invalidate(false, true, true);
}

void CMesh::AddVertices(const CString & name, CString const & type, short dim, void * data)
{
	auto e = new CVertexes();
	Vertexes.push_back(e);

	e->Name = name;
	e->Type = type;
	e->Dim = dim;
	e->Insert(data);

	Realization->Invalidate(true, false, false);
}

void CMesh::SetVertices(const CString & name, CArray<CFloat2> & v)
{
	auto e = Vertexes.Find([&name](auto i){ return i->Name == name; });

	if(e)
	{
		if(e->Name != name)
			throw CException(HERE, L"Vertex element not found");
		if(e->Type != CFloat::TypeName)
			throw CException(HERE, L"Vertex element type mismatch");
		if(e->Dim != 2)
			throw CException(HERE, L"Vertex element dimention mismatch");
	
		((CArray<CFloat2> *)e->Data)->assign(v.begin(), v.end());
	
		Realization->Invalidate(false, true, false);
	} 
	else
	{
		auto e = new CVertexes();
		Vertexes.push_back(e);

		e->Name = name;
		e->Type = CFloat::TypeName;
		e->Dim = 2;
		e->Data = new CArray<CFloat2>(v);
	
		Realization->Invalidate(true, false, false);
	}
}

void CMesh::SetVertices(const CString & name, CArray<CFloat3> & v)
{
	auto e = Vertexes.Find([&name](auto i){ return i->Name == name; });

	if(e)
	{
		if(e->Name != name)
			throw CException(HERE, L"Vertex element not found");
		if(e->Type != CFloat::TypeName)
			throw CException(HERE, L"Vertex element type mismatch");
		if(e->Dim != 3)
			throw CException(HERE, L"Vertex element dimention mismatch");
	
		((CArray<CFloat3> *)e->Data)->assign(v.begin(), v.end());
	
		Realization->Invalidate(false, true, false);
	} 
	else
	{
		auto e = new CVertexes();
		Vertexes.push_back(e);
	
		e->Name = name;
		e->Type = CFloat::TypeName;
		e->Dim = 3;
		e->Data = new CArray<CFloat3>(v);

		Realization->Invalidate(true, false, false);
	}
}

void CMesh::SetVertices(const CString & name, CArray<int> & v)
{
	auto e = Vertexes.Find([&name](auto & i){ return i->Name == name; });

	if(e)
	{
		if(e->Name != name)
			throw CException(HERE, L"Vertex element not found");
		if(e->Type != CInt32::TypeName)
			throw CException(HERE, L"Vertex element type mismatch");
		if(e->Dim != 1)
			throw CException(HERE, L"Vertex element dimention mismatch");
	
		((CArray<int> *)e->Data)->assign(v.begin(), v.end());
	
		Realization->Invalidate(false, true, false);
	} 
	else
	{
		auto e = new CVertexes();
		Vertexes.push_back(e);

		e->Name = name;
		e->Type = CInt32::TypeName;
		e->Dim = 1;
		e->Data = new CArray<int>(v);

		Realization->Invalidate(true, false, false);
	}
}

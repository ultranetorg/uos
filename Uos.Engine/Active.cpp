#include "StdAfx.h"
#include "Active.h"
#include "ActiveGraph.h"

using namespace uos;

CPick::CPick(CPick && a) : Camera(a.Camera), Ray(a.Ray), CMeshIntersection(a)
{
	Space		= a.Space;
	Active		= a.Active;
	Mesh		= a.Mesh;
	Camera		= a.Camera;
	Vpp			= a.Vpp;
	Ray			= a.Ray;
	Z			= a.Z;

	a.Space = null;
	a.Active = null;
	a.Mesh = null;
}

CPick::CPick(const CPick & a)
{
	Camera		= a.Camera;
	Vpp			= a.Vpp;
	Ray			= a.Ray;
	Z			= a.Z;

	Distance	= a.Distance;
	StartIndex	= a.StartIndex;
	Point		= a.Point;
	Space		= sh_assign(Space, a.Space);
	Active		= sh_assign(Active, a.Active);
	Mesh		= sh_assign(Mesh, a.Mesh);
}

CPick::~CPick()
{
	if(Space)
		Space->Free();

	if(Active)
		Active->Free();

	if(Mesh)
		Mesh->Free();
}

CFloat3 CPick::GetFinalPosition()
{
	return (Active->FinalMatrix).TransformCoord(Point);
}

CFloat3 CPick::GetWorldPosition()
{
	return (Active->FinalMatrix * Space->Matrix).TransformCoord(Point);
}

bool CPick::operator != (const CPick & a)
{
	return Space != a.Space || Camera != a.Camera || Vpp != a.Vpp || Active != a.Active || Mesh != a.Mesh || CMeshIntersection::operator!=(a);
}

CPick & CPick::operator=(const CPick & a)
{
	Camera		= a.Camera;
	Vpp			= a.Vpp;
	Ray			= a.Ray;
	Z			= a.Z;

	Distance	= a.Distance;
	StartIndex	= a.StartIndex;
	Point		= a.Point;
	Space		= sh_assign(Space, a.Space);
	Active		= sh_assign(Active, a.Active);
	Mesh		= sh_assign(Mesh, a.Mesh);

	return *this;
}
	
/////////////////

CActive::CActive(CEngineLevel * l, const CString & name) : CEngineEntity(l)
{
	SetName(name);
}
	
CActive::~CActive()
{
	if(Mesh)		
		Mesh->Free();

	//if(Space)
	//	DebugBreak();
		
	if(ClippingMesh)
		ClippingMesh->Free();

	for(auto & i : Metas)
	{
		sh_free(i.Shared);
	}
}
	
CString CActive::GetStatus()
{
	CString a;
	a += Enabled ? L"E " : L"  ";
	a += FinalMatrix.IsReal() ? L"MX " : L"mx "; 
	a += Mesh ? (Mesh->IsReady() ? L"MH " : L"mh ") : L"   ";
	a += Graph ? L"G " : L"";
	return a;
}
	
bool CActive::IsReady()
{
	return Mesh && Mesh->IsReady() && FinalMatrix.IsReal();
}

void CActive::Enable(bool e)
{
	Enabled = e;
}

void CActive::SetName(const CString & name)
{
	Name = name;
	//Name = name.substr(0, name.find('-'));
	
#ifdef _DEBUG
	FullName = Parent ? Parent->FullName + L"/" + name : name;

	for(auto i : Nodes)
	{
		i->SetName(i->Name);
	}
#endif

}

void CActive::SetMesh(CMesh * mesh)
{
	Mesh = sh_assign(Mesh, mesh);
}

void CActive::SetMatrix(CMatrix const & m)
{
	Matrix = m;

	CMatrix fm;

	if(Parent && Transformating == ETransformating::Inherit)
	{
		fm = m * Parent->FinalMatrix;
	}
	else
	{
		fm = m;
	}

	if(fm != FinalMatrix)
	{
		//Matrix = m;
		FinalMatrix = fm;

		#ifdef _DEBUG
		_Decomposed = FinalMatrix.Decompose();
		#endif

		for(auto i : Nodes)
		{
			i->SetMatrix(i->Matrix);
		}
	}
}

CMatrix & CActive::GetMatrix()
{
	return FinalMatrix;
}

void CActive::SetMeta(const CString & k, const void * v)
{
	Metas.push_back(CMetaItem(k, v));
}

void CActive::SetMeta(const CString & k, const CString & v)
{
	Metas.push_back(CMetaItem(k, v));
}

void * CActive::GetMetaPointer(const CString & k)
{
	return Metas.Find([k](auto & i){ return i.Key == k; }).Pointer;
}
	
CString & CActive::GetMetaString(const CString & k)
{
	return Metas.Find([k](auto & i){ return i.Key == k; }).String;
}
	
CMesh * CActive::GetMesh()
{
	return Mesh;
}

EActiveState CActive::GetState()
{
	return State;
}

void CActive::AddNode(CActive * n)
{
	Nodes.Add(n);
	n->Parent = this;
	n->Graph = Graph;
	n->SetMatrix(n->Matrix);

#ifdef _DEBUG
	n->SetName(n->Name);
#endif // _DEBUG

	if(Graph)
	{
		n->Attach();
	}
}

void CActive::RemoveNode(CActive * n)
{
	if(n->Graph)
	{
		n->Detach();
		Graph->OnNodeDetach(n);
	}

	Nodes.Remove(n);
	n->Parent = null;
	n->Graph = null;

#ifdef _DEBUG
	n->SetName(n->Name);
#endif // _DEBUG
}

void CActive::Attach()
{
	Graph = Parent->Graph;

	for(auto i : Nodes)
	{
		i->Attach();
	}
}

void CActive::Detach()
{
	Graph = null;
	//State = EActiveState::Normal;

	for(auto i : Nodes)
	{
		i->Detach();
	}
}

CActive * CActive::GetParent()
{
	return Parent;
}

CActive * CActive::GetAncestor(const CString & name)
{
	auto a = Parent;
	while(a != null && a->Name != name)
	{
		a = a->Parent;
	}
	return a;
}

CActive * CActive::GetRoot()
{
	CActive * p = this;
	while(p->Parent != null)
	{
		p = p->Parent;
	}
	return p;
}

bool CActive::HasAncestor(CActive * n)
{
	CActive * p = this;
	while(p && p != n)
	{
		p = p->Parent;
	}
	return p != null;
}

CActive * CActive::GetElderEnabled()
{
	auto p = this;
	while(p != null && p->FindFirstDisabledAncestor() != null)
	{
		p = p->FindFirstDisabledAncestor()->Parent;
	}
	return p;
}

CActive * CActive::FindFirstDisabledAncestor()
{
	CActive * p = this;
	while(p!=null && p->Enabled)
	{
		p = p->Parent;
	}
	return p;
}

bool CActive::IsFinallyEnabled()
{
	CActive * n = this;
	while(n != null)
	{
		if(!n->Enabled)
		{
			return false;
		}
		n = n->Parent;
	}
	return true;
}

CActive * CActive::FindCommonAncestor(CActive * n)
{
	CActive * p0 = this;
	while(p0 != null)
	{
		CActive * p1 = n;
		while(p1 != null)
		{
			if(p0 == p1)
			{
				return p0;
			}
			p1 = p1->Parent;
		}
		p0 = p0->Parent;
	}
	return null;
}

/*
CInteractive * CInteractive::GetFocus()
{
	CInteractive * c = ActiveChild;
	while(c!=null)
	{
		c = c->ActiveChild;
	}
	return c;
}*/

CActive * CActive::FindChildContaning(CActive * a)
{
	auto n = a;
	while(n != null && n->Parent != this)
	{
		n = n->Parent;
	}
	return n;
}

CFloat3 CActive::Transit(CActive * owner, CFloat3 & p)
{
	return (owner->FinalMatrix * !FinalMatrix).TransformCoord(p);
}

CActive * CActive::GetActualClipper()
{
	if(GetActualClipping() == EClipping::No)
	{
		return null;
	}

	auto p = Parent; // find nearest parent with clipper
	while(p)
	{
		if(p->ClippingMesh && p->ClippingMesh->IsReady())
		{
			return p;
		}
		p = p->Parent;
	}

	return p; // find nearest valid clipper
}


EClipping CActive::GetActualClipping()
{
	auto p = this; // find nearest parent with clipper
	while(p && p->Clipping == EClipping::Inherit)
		p = p->Parent;

	return p ? p->Clipping : EClipping::Inherit; // find nearest valid clipper
}

void CActive::SetClipping(CMesh * mesh)
{
	ClippingMesh = sh_assign(ClippingMesh, mesh);
}

CMesh * CActive::GetClipping()
{
	return ClippingMesh;
}
	
CAABB CActive::GetClippingBBox2D()
{
	auto bb = ClippingMesh->BBox.Transform(FinalMatrix);

	bb.Min.z = bb.Max.z = 0;

	auto c = GetActualClipper();

	if(c)
	{
		bb = bb.Cross(c->GetClippingBBox2D());
	}

	return bb;
}


CAABB CActive::GetFinalBBox2D()
{
	auto bb = Mesh->BBox.Transform(FinalMatrix);

	bb.Min.z = bb.Max.z = 0;

	if(Clipping == EClipping::Inherit || Clipping == EClipping::Apply)
	{
		auto c = GetActualClipper();
		if(c)
		{
			bb = bb.Cross(c->GetClippingBBox2D());
		}	
	}
	

	return bb;
}

CMeshIntersection CActive::GetIntersection(CRay & r)
{
	CMatrix invm = FinalMatrix.GetInversed();

	CRay ray;
	ray.Origin		= invm.TransformCoord(r.Origin);
	ray.Direction	= invm.TransformNormal(r.Direction).GetNormalized();


	return Mesh->Intersect(ray);
}

CFloat3 CActive::GetXyPlaneIntersectionPoint(CMatrix & sm, CCamera * c, CFloat2 & vpp)
{
	auto r = c->Raycast(vpp);
		
	return CPlane(0, 0, -1).Intersect(r.Transform((FinalMatrix * sm).GetInversed()));
}

void CActive::TransformMatrix(CMatrix & m)
{
	Matrix *= m;
	SetMatrix(Matrix);
}

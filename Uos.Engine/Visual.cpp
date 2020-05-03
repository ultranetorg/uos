#include "StdAfx.h"
#include "Visual.h"
#include "Pipeline.h"

using namespace uos;

CVisual::CVisual(CEngineLevel * l, const CString & name, CMesh * mesh, CMaterial * mtl, const CMatrix & m) : CEngineEntity(l)
{
	SetName(name);
	SetMesh(mesh);
	SetMaterial(mtl);
	SetMatrix(m);
}

CVisual::~CVisual()
{
	delete Buffer;

	if(Pipeline)
		Pipeline->Free();

	if(Mesh)		
		Mesh->Free();
	
	if(Material)		
		Material->Free();

	if(InheritableMaterial)		
		InheritableMaterial->Free();

	if(ClippingMesh)
		ClippingMesh->Free();
}

CString CVisual::GetStatus()
{
	CString a;
	a += Enabled ? L"+ " : L"- ";
	a += Pipeline ? L"PPL " : L"    ";
	a += Mesh ? (Mesh->IsReady() ? L"MSH " : L"msh ") : L"    ";
	a += Material ? L"MTL " : L"    ";
	a += Matrix.IsReal() ? L"MTX " : L"mtx "; 
	//a += View ? L"V  " : (GetActualView() ? L"V' " : L"  ");
	return a;
}

CString CVisual::GetClippingStatus()
{
	return CString::Format(L"%-8s  %p  %3d  %s", ToString(Clipping), GetActualClipping() == EClipping::Apply ? GetActualClipper() : null, StencilIndex, ClippingMesh ? L"CLMSH" : L"");
}

void CVisual::Save(CXon * r, IMeshStore * sm, IMaterialStore * st)
{
	r->Set(Name);

	if(Mesh)
		r->Add(L"Mesh")->Set(sm->Add(Mesh));

	if(ClippingMesh)
		r->Add(L"ClippingMesh")->Set(sm->Add(ClippingMesh));

	if(Material)
		r->Add(L"Material")->Set(st->Add(Material));

	///if(GetActualView())
	///	r->Add(L"View")->Set(GetActualView()->GetName());

	r->Add(L"Matrix")->Set(Matrix);

	if(Clipping != EClipping::Inherit)
		r->Add(L"Clipping")->Set((int)Clipping);

	if(PixelPrecision)
		r->Add(L"PixelPrecision")->Set(PixelPrecision);

	for(auto i : Nodes)
	{
		i->Save(r->Add(L"Visual"), sm, st);
	}
}

void CVisual::Load(CXon * r, IMeshStore * meshs, IMaterialStore * mtls, CMaterialFactory * f)
{
	Name = r->AsString();
	Matrix = r->One(L"Matrix")->Get<CMatrix>();
	
	auto p = r->One(L"Mesh");
	if(p)
	{
		SetMesh(meshs->Get(p->Get<CString>()));
	}

	p = r->One(L"ClippingMesh");
	if(p)
	{
		SetClipping(meshs->Get(p->Get<CString>()));
	}
	   	
	p = r->One(L"Material");
	if(p)
	{
		SetMaterial(mtls->GetMaterial(p->Get<CString>()));
	}	

	///if(p)
	///p = r->One(L"View");
	///{
	///	View = vs->Get(p->AsString());
	///}	

	p = r->One(L"Clipping");
	if(p)
	{
		Clipping = (EClipping)p->AsInt32();
	}	

	p = r->One(L"PixelPrecision");
	if(p)
	{
		PixelPrecision = p->AsBool();
	}	
	
	for(auto i : r->Many(L"Visual"))
	{
		auto v = new CVisual(Level, L"", null, null, CMatrix());
		v->Load(i, meshs, mtls, f);
		AddNode(v);
		v->Free();
	}
}

CVisual * CVisual::Clone()
{
	auto v = new CVisual(Level, Name, Mesh, Material, Matrix);
	v->Clipping = Clipping;
	v->PixelPrecision = PixelPrecision;
	v->ClippingMesh = sh_assign(v->ClippingMesh, ClippingMesh);

	for(auto i : Nodes)
	{
		auto j = i->Clone();
		v->AddNode(j);
		j->Free();
	}

	return v;
}

void CVisual::SetName(const CString & name)
{
	Name = name;
	//Name = name.substr(0, name.find('-'));
	#ifdef _DEBUG
	FullName = Parent ? Parent->FullName + L"/" + Name : Name;
	#endif
		
	for(auto i : Nodes)
	{
		i->SetName(i->Name);
	}
}

CString & CVisual::GetName()
{
	return Name;
}

bool CVisual::IsReady()
{
	return Enabled && Material && Mesh && Mesh->IsReady();
}

void CVisual::SetMaterial(CMaterial * m)
{
	if((!!Material != !!m) || (Material && m && Material->Shader != m->Shader))
	{
		ShaderChanged = true;
	}

	Material = sh_assign(Material, m);

	if(m)
	{
		if(!Buffer)
			Buffer = new CDirectConstantBuffer();
	
		Buffer->Bind(Material->Shader, L"Geometry");
	
		CameraPositionSlot	= Buffer->FindSlot(L"CameraPosition");
		WSlot				= Buffer->FindSlot(L"W");
		WVPSlot				= Buffer->FindSlot(L"WVP");
	}
	else
	{
		delete Buffer;
		Buffer = null;
	}
}

void CVisual::SetMesh(CMesh * mesh)
{
	Mesh = sh_assign(Mesh, mesh);
	Updates |= EVisualUpdate_Obb;
}

void CVisual::SetInheritableMaterial(CMaterial * material)
{
	InheritableMaterial = sh_assign(InheritableMaterial, material);
}

CMaterial * CVisual::GetMaterial()
{
	return Material;
}	

CMesh * CVisual::GetMesh()
{
	return Mesh;
}

void CVisual::Enable(bool e)
{
	if(Enabled != e)
	{
		//			MarkAsModifiedRecursively();
	}
	Enabled = e;
}

bool CVisual::IsEnabled()
{
	return Enabled;
}

void CVisual::SetMatrix(const CMatrix & m)
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
		if(PixelPrecision)
		{
			fm._41 = floor(fm._41);
			fm._42 = floor(fm._42);
		}

		FinalMatrix = fm;

		#ifdef _DEBUG
		_Decomposed = Matrix.Decompose();
		#endif

		for(auto i : Nodes)
		{
			i->SetMatrix(i->Matrix);
		}

		//MatrixModified();
		Updates |= EVisualUpdate_Obb;
		FinalMatrixChanged();
	}
}

void CVisual::SetMatrixPosition(CFloat3 & p)
{
	Matrix._41 = p.x;
	Matrix._42 = p.y;
	Matrix._43 = p.z;
	SetMatrix(Matrix);
}

void CVisual::TransformMatrix(CMatrix & m)
{
	Matrix *= m;
	SetMatrix(Matrix);
	Updates |= EVisualUpdate_Obb;
}

CMatrix & CVisual::GetMatrix()
{
	return FinalMatrix;
}

CAABB CVisual::GetClippingBBox(const CMatrix & m)
{
	auto bb = ClippingMesh->BBox.Transform(FinalMatrix * m);

	auto c = GetActualClipper();
	if(c)
	{
		bb = bb.Cross(c->GetClippingBBox(m));
	}		

	return bb;
}

CAABB CVisual::GetFinalBBox(const CMatrix & m)
{
	auto bb = Mesh->BBox.Transform(FinalMatrix * m);

	auto c = GetActualClipper();
	if(c)
	{
		bb = bb.Cross(c->GetClippingBBox(m));
	}		

	return bb;
}

void CVisual::SetCullMode(ECullMode c)
{
	CullMode = c;
}

ECullMode CVisual::GetCullMode()
{
	return CullMode;
}

CVisual * CVisual::GetActualClipper()
{
	auto p = Parent; // find nearest parent with clipper
	while(p && (p->ClippingMesh == null || !p->ClippingMesh->IsReady()))
		p = p->Parent;

	return p; // find nearest valid clipper
}

EClipping CVisual::GetActualClipping()
{
	auto p = this; // find nearest parent with clipper
	while(p && p->Clipping == EClipping::Inherit)
		p = p->Parent;

	return p ? p->Clipping : EClipping::Inherit; // find nearest valid clipper
}

void CVisual::SetClipping(CMesh * mesh)
{
	ClippingMesh = sh_assign(ClippingMesh, mesh);
}

CMesh * CVisual::GetClipping()
{
	return ClippingMesh;
}

void CVisual::AddNode(CVisual * v)
{
	v->Parent = this;
	Nodes.Add(v);

	v->SetMatrix(v->Matrix);
	v->SetName(v->Name);
}

void CVisual::InsertNode(CVisual * v, CList<CVisual *>::iterator where)
{
	v->Parent = this;
	Nodes.Insert(v, where);

	v->SetMatrix(v->Matrix);
	v->SetName(v->Name);
}

void CVisual::RemoveNode(CVisual * v)
{
	v->Parent = null;
	v->SetName(v->Name);
	Nodes.Remove(v);

}

CAABB CVisual::GetAABB()
{
	std::function<CAABB(CVisual * v, CMatrix const & m)> f =[&f](CVisual * v, CMatrix const & m)
															{
																CAABB bb = CAABB::Empty;

																if(v->Mesh)
																{
																	bb = v->Mesh->GetBBox().Transform(m);

																	///auto c = v->GetActualClipper();
																	///if(c)
																	///{
																	///	 auto a = bb.Cross(c->GetClippingBBox(m));
																	///	 //auto b = bb.Join(c->GetClippingBBox(m));
																	///	 bb.Min.x = a.Min.x;
																	///	 bb.Min.y = a.Min.y;
																	///	 //bb.Min.z = b.Min.z;
																	///
																	///	 bb.Max.x = a.Max.x;
																	///	 bb.Max.y = a.Max.y;
																	///	 //bb.Max.z = b.Max.z;
																	///}	
																}

																for(auto i : v->Nodes)
																{
																	bb = bb.Join(f(i, i->Matrix * m));
																}

																return bb;
															};
	
	CMatrix m = Matrix;
	m._41 = m._42 = m._43 = 0;
	return f(this, m);
}

CRect CVisual::GetProjectionRect(CCamera * c)
{
	auto bb = GetFinalBBox(CMatrix::Identity);

	auto min = c->ProjectVertex(bb.Min);
	auto max = c->ProjectVertex(bb.Max);

	return CRect(min.x, min.y, max.x - min.x, max.y - min.y);

}

void CVisual::Apply(CDirectDevice * d, CMatrix & sm, CCamera * c)
{
	if(CameraPositionSlot != -1)	Buffer->SetValue(CameraPositionSlot, c->Position);
	if(WSlot != -1)					Buffer->SetValue(WSlot, FinalMatrix);
	if(WVPSlot != -1)				Buffer->SetValue(WVPSlot, FinalMatrix * sm * c->GetViewProjectionMatrix());

	Buffer->Apply(d);
}


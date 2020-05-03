#include "StdAfx.h"
#include "Rectangle.h"

using namespace uos;

CRectangle::CRectangle(CWorldLevel * l, const CString & name) : CElement(l, name)
{
	InnerMesh = null;
}
	
CRectangle::~CRectangle()
{
	//for(auto i : Nodes)
	//{
	//	Nodes.Remove(i);
	//	i->Parent = null;
	//
	//	VContent->RemoveNode(i->Visual);
	//	AContent->RemoveNode(i->Active);
	//
	//	i->Free();
	//}
	//Nodes.clear();

	if(VBorder)
	{
		Visual->RemoveNode(VBorder);
		VBorder->Free();
	}

	if(VInner)
	{
		VInner->Free();
	}
	//if(VContent)
	//{
	//	Visual->RemoveNode(VContent);
	//	VContent->Free();
	//}
	//if(AContent)
	//{
	//	Active->RemoveNode(AContent);
	//	AContent->Free();
	//}
	if(InnerMesh)
	{
		InnerMesh->Free();
	}
}

void CRectangle::UseInner()
{
	if(!InnerMesh)
	{
		InnerMesh = new CSolidRectangleMesh(&Level->Engine->EngineLevel);
	}
	
	VInner = new CVisual(&Level->Engine->EngineLevel, L"canvas", null, null, CMatrix(0, 0, Z_STEP));
	VInner->SetMesh(InnerMesh);
	Visual->AddNode(VInner);
}

void CRectangle::UseCanvas(CShader * s)
{
	UseInner();
	CElement::UseCanvas(VInner, InnerMesh, s);
}

void CRectangle::UseClipping(EClipping c, bool content)
{
	Visual->Clipping = c;
	Active->Clipping = c;

	if(content)
	{
		if(!InnerMesh)
		{
			InnerMesh = new CSolidRectangleMesh(&Level->Engine->EngineLevel);
		}
	
		Visual->SetClipping(InnerMesh);
		Active->SetClipping(InnerMesh);
	}
}

void CRectangle::UpdateLayout(CLimits const & l, bool apply)
{
	__super::UpdateLayout(l, apply);

	if(apply && W > 0 && H > 0)
	{
		ApplyBody();
		ApplyBorder();
		UpdateCanvas();
	}
}

void CRectangle::ApplyBody()
{
	if(W > 0 && H > 0)
	{
		if(!Visual->Mesh)
		{
			auto m = new CSolidRectangleMesh(&Level->Engine->EngineLevel);
			Visual->SetMesh(m);
			m->Free();
		}

		if(!Active->Mesh && EnableActiveBody)
		{
			auto m = new CSolidRectangleMesh(&Level->Engine->EngineLevel);
			Active->SetMesh(m);
			m->Free();
		}

		Visual->Mesh->As<CSolidRectangleMesh>()->Generate(M.LF + B.LF, M.BM + B.BM, W - (M.LF + M.RT + B.LF + B.RT), H - (M.BM + M.TP + B.BM + B.TP));

		if(InnerMesh)
			InnerMesh->Generate(O.x, O.y, IW, IH);

		if(Active->Mesh)
			Active->Mesh->As<CSolidRectangleMesh>()->Generate(0.f, 0.f, W, H);
	}
}

void CRectangle::ApplyBorder()
{
	if(B.LF > 0.f || B.RT > 0.f || B.TP > 0.f || B.BM > 0.f)
	{
		if(VBorder == null)
		{
			VBorder = new CVisual(&Level->Engine->EngineLevel, L"border", null, null, CMatrix());
			//VBorder->Clipping = true;;
			VBorder->SetMaterial(BorderMaterial);
			VBorder->SetMatrix(CMatrix::Identity);
			Visual->AddNode(VBorder);

			auto m = new CFrameMesh(&Level->Engine->EngineLevel);
			VBorder->SetMesh(m);
			m->Free();
		}

		//VBorder->Clipping = Visual->Clipping;
		auto w = M.LF + B.LF + M.RT + B.RT;
		auto h = M.BM + B.BM + M.TP + B.TP;

		VBorder->Mesh->As<CFrameMesh>()->Generate(M.LF, M.BM, 0, W - w, H - h, B.LF, B.BM, B.RT, B.TP);
	}
	else
	{
		if(VBorder != null)
		{
			Visual->RemoveNode(VBorder);
			VBorder->Free();
			VBorder = null;
		}
	}
}

void CRectangle::LoadProperties(CStyle * s, CXon * n)
{
	__super::LoadProperties(s, n);
	
	for(auto i : n->Children)
	{
		if(i->Name == L"Background")
			for(auto j : i->Children)
			{
				if(j->Name == L"Material")
					Visual->SetMaterial(Level->Materials->GetMaterial(j->Get<CString>()));
			}
		else if(i->Name == L"Border")
			for(auto j : i->Children)
			{
				if(j->Name == L"Material")
					BorderMaterial = Level->Materials->GetMaterial(j->Get<CString>());
				else if(j->Name == L"Thickness")
				{
					auto t = CFloat6(j->Get<CString>());
					Express(L"B", [t]{ return t; });
				}
			}
	}
}

#include "stdafx.h"
#include "RectangleSectionMenuItem.h"

using namespace uos;

CRectangleSectionMenuItem::CRectangleSectionMenuItem(CWorldLevel * w, CStyle * s, CString const & title, const CString & name) : CRectangleTextMenuItem(w, s, null, title)
{
	auto mesh = s->GetMesh(L"RectangleSectionMenuItem/Arrow");

	Arrow = new CElement(Level, L"Arrow");
	Arrow->Express(L"W", [this, mesh]{ return mesh->GetBBox().GetSize().W; });
	Arrow->Express(L"H", [this, mesh]{ return mesh->GetBBox().GetSize().H; });
	Arrow->Visual->SetMesh(mesh);
	Arrow->Visual->SetMaterial(Style->GetMaterial(L"Menu/Text/NormalColor"));
	AddNode(Arrow); // needed here to hold refs
	Arrow->Free();

	//SetupContent();
	
	auto ms = new CRectangleMenuSection(w, Style);
	SetSection(ms);
	ms->Free();
}

CRectangleSectionMenuItem::~CRectangleSectionMenuItem()
{
	if(Section)
	{
		Section->Free();
	}
	RemoveNode(Arrow);
}

CMesh * CRectangleSectionMenuItem::CreateArrowMesh(CWorldLevel * world)
{
	auto m = new CMesh(&world->Engine->EngineLevel);

	CArray<CFloat3> a(3);

	float w = 4, h = 8;

	a[0].x = 0;			a[1].x = w;
	a[0].y = h;			a[1].y = h/2;
	a[0].z = 0;			a[1].z = 0;

	a[2].x = 0;
	a[2].y = 0;
	a[2].z = 0;

	m->SetVertices(UOS_MESH_ELEMENT_POSITION, a);

	CArray<int> ix(3);
	ix[0] = 0;
	ix[1] = 1;
	ix[2] = 2;


	m->SetIndexes(ix);
	m->SetPrimitiveInfo(EPrimitiveType::TriangleList);	
	m->SetBBox(CAABB(w, h, 0));

	return m;
}

void CRectangleSectionMenuItem::SetSection(IMenuSection * m)
{
	Section = sh_assign(Section, dynamic_cast<CRectangleMenuSection *>(m));
}

void CRectangleSectionMenuItem::HighlightArrow(bool e, CSize & area)
{
	Arrow->Visual->SetMaterial(Level->Materials->GetMaterial(Label->Color));
	//Arrow->SetMaterial(Text->Visual->Material);
//	Arrow->SetShader(Text->Visual->Shader);
}

void CRectangleSectionMenuItem::Highlight(CArea * a, bool e, CSize & s, CPick * p)
{
	__super::Highlight(a, e, s, p);

	HighlightArrow(e, s);

	if(e)
	{
		Section->Open(a, this, p, 0, W, H, 0);
	}
	else
	{
		Section->Close();
	}
}






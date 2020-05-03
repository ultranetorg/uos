#include "stdafx.h"
#include "RectangleMenuSection.h"
#include "RectangleSectionMenuItem.h"

using namespace uos;

CRectangleMenuSection::CRectangleMenuSection(CWorldLevel * w, CStyle * s, const CString & name) : CRectangle(w, name)
{
	Style = s;
	Express(L"IW",	[this](){ return C.W; });
	Express(L"IH",	[this](){ return C.H; });
	Express(L"P",	[this](){ return CFloat6(2); });
	Express(L"B",	[this](){ return CFloat6(1); });
	Express(L"C",	[this](auto apply)
					{
						auto bb = CAABB::InversedMax;

						//float w = 0.f;
						for(auto it = Items.rbegin(); it != Items.rend(); it++)
						{
							auto i = *it;
							i->Express(L"IW", [i]{ return i->C.W; });
							i->UpdateLayout(Climits, false);
							i->Transform(0, bb.GetSize().H, Z_STEP * 2);

							bb.Join2D(i->Transformation, i->Size);
						}
	
						if(apply)
						{
							auto a = bb.GetSize();
							a.W = min(a.W, Climits.Smax.W);
							//a.H = a.H;
	
							for(auto i : Items)
							{
								i->Express(L"W", [a]{ return a.W; });
								i->UpdateLayout(CLimits(a, a), apply);
							}
						}
						return bb.GetSize();
					});

	Visual->SetMaterial(Level->Materials->GetMaterial(Style->Get<CFloat4>(L"Menu/Background/Color")));
	BorderMaterial = Level->Materials->GetMaterial(Style->Get<CFloat4>(L"Menu/Border/Color"));
	
	Active->MouseInput	+= ThisHandler(OnMouse);
	Active->StateChanged += ThisHandler(OnStateModified);

	UseClipping(EClipping::No, true);

	auto m = new CSolidRectangleMesh(&Level->Engine->EngineLevel);
	Highlighter = new CVisual(&Level->Engine->EngineLevel, L"highlighter", m, BorderMaterial, CMatrix());
	m->Free();
}

CRectangleMenuSection::~CRectangleMenuSection()
{
	Highlighter->Free();

	for(auto i : Items)
	{
		RemoveNode(i);
	}

	Active->StateChanged -= ThisHandler(OnStateModified);
}

void CRectangleMenuSection::AddItem(IMenuItem * item)
{
	auto i = dynamic_cast<CRectangleTextMenuItem *>(item);

	i->Active->MouseInput += ThisHandler(OnItemCursorMoved);
	Items.Add(i);
	AddNode(i);
}

IMenuItem * CRectangleMenuSection::AddItem(const CString & title)
{
	return AddStandardItem(null, title);
}

CRectangleTextMenuItem * CRectangleMenuSection::AddStandardItem(CTexture * icon, CString const & text)
{
	auto i = new CRectangleTextMenuItem(Level, Style, icon, text);
	AddItem(i);
	i->Free();
	return i;
}

ISectionMenuItem * CRectangleMenuSection::AddSectionItem(CString const & text)
{
	auto i = new CRectangleSectionMenuItem(Level, Style, text);
	AddItem(i);
	i->Free();
	return i;
}

IMenuItem * CRectangleMenuSection::AddSeparator()
{
	auto i = new CRectangleSeparatorMenuItem(Level, Style);
	Items.Add(i);
	AddNode(i);
	i->Free();
	return i;
}

void CRectangleMenuSection::RemoveItem(IMenuItem * item)
{
	auto i = dynamic_cast<CRectangleTextMenuItem *>(item);

	i->Active->MouseInput -= ThisHandler(OnItemCursorMoved);
	Items.Remove(i);
	RemoveNode(i);
}

void CRectangleMenuSection::Clear()
{
	for(auto i : Items)
	{
		i->Active->MouseInput -= ThisHandler(OnItemCursorMoved);
		RemoveNode(i);
	}
	Items.Clear();
}

void CRectangleMenuSection::OnItemCursorMoved(CActive * r, CActive * s, CMouseArgs * arg)
{
	if(arg->Event == EGraphEvent::Enter)
	{
		auto mi = s->GetOwnerAs<CRectangleMenuItem>();

		if(!Highlighter->Parent)
		{
			Visual->InsertNode(Highlighter, Visual->Nodes.begin());
		}

		if(Highlighted != mi)
		{
			if(Highlighted)
			{
				Highlighted->Highlight(Area, false, CSize(), &arg->Pick);
			}
	
			if(mi)
			{
				mi->Highlight(Area, true, CSize(IW, mi->H, 0), &arg->Pick);
				Highlighter->Mesh->As<CSolidRectangleMesh>()->Generate(0, 0, IW, mi->H);
				Highlighter->SetMatrix(CMatrix::FromPosition(O.x + mi->Transformation.Position.x, O.y + mi->Transformation.Position.y, Z_STEP));
			}
	
			Highlighted = sh_assign(Highlighted, mi);
		}
	}
}

void CRectangleMenuSection::Unhighlight()
{
	if(Highlighted)
	{
		Highlighted->Highlight(null, false, CSize(), null);
		sh_free(Highlighted);
	}

	if(Highlighter->Parent)
	{
		Visual->RemoveNode(Highlighter);
	}
}

void CRectangleMenuSection::OnMouse(CActive * r, CActive * s, CMouseArgs * a)
{
	if(a->Event == EGraphEvent::Leave)
	{
		if(Highlighted && Highlighted->As<ISectionMenuItem>() == null)
		{
			if(Highlighter->Parent)
			{
				Visual->RemoveNode(Highlighter);
			}

			Highlighted->Highlight(Area, false, CSize(), &a->Pick);
			sh_free(Highlighted);
		}
	}
}

void CRectangleMenuSection::OnStateModified(CActive * r, CActive * s, CActiveStateArgs * a)
{
	if(s == Active && Highlighter && a->Old == EActiveState::Active)
	{
		if(Highlighter->Parent)
		{
			Visual->RemoveNode(Highlighter);

			if(Highlighted)
			{
				Highlighted->Highlight(Area, false, CSize(), null);
				sh_free(Highlighted);
			}
		}
	}
}
		
void CRectangleMenuSection::Open(CArea * a, CElement * parent, CPick * pick, float l, float r, float u, float d) // uses parent viewport
{
	Area = a;

	Opening(this);

	parent->AddNode(this);

//	a->Area->Positioning->

	auto smax = a->GetActualMaxSize(pick->Camera->Viewport);

	UpdateLayout(CLimits(smax, smax), true);

	auto p = CFloat3(r, u - H, 0);
	
	//Transform(p);
	auto s = p.VertexTransform(parent->Visual->FinalMatrix * a->VisualSpaces.Match(pick->Camera->Viewport).Space->Matrix);//(CMatrix(p.x, p.y, p.z) * pnt->Visual->FinalMatrix).GetPosition();

	auto ml = s.x;
	auto mr = s.x + W;
	auto md = s.y;
	auto mu = s.y + H;

	
	if(mr > smax.W/2)
		p.x = l - W;

	if(md < -smax.H/2)
		p.y = d;

	//Transform(p);
	s = parent->Visual->FinalMatrix.TransformCoord(p);

	if(s.y + H > smax.H/2)
		p.y -= (s.y + H) - smax.H/2 + 1;
	
	//if(mu > a.Area.H/2)
	//	p.y -= mu - a.Area.H/2;


	p.z = -1;

	Transform(p);
}

void CRectangleMenuSection::Open(CElement * pnt)
{
	Opening(this);

	pnt->AddNode(this);

	UpdateLayout(CLimits::Max, true);
}

void CRectangleMenuSection::Close()
{
	Closing(this);

	if(Highlighted)
	{
		Highlighted->Highlight(Area, false, CSize(), null);
		sh_free(Highlighted);
	}

	if(Highlighter->Parent)
	{
		Visual->RemoveNode(Highlighter);
	}

	if(Parent)
	{
		Parent->RemoveNode(this);
	}
}

bool CRectangleMenuSection::IsOpen()
{
	return Parent != null;
}


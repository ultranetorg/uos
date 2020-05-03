#include "stdafx.h"
#include "Grid.h"

using namespace uos;

CGrid::CGrid(CWorldLevel * l, CStyle * s, CString const & name) : CRectangle(l, name)
{
	Style = s;
		
	Header = new CGridHeader(l, s);
	Header->Spacing = Spacing;
	AddNode(Header);

	Scrollbar = new CScrollbar(l, s);
	Scrollbar->Express(L"W", []{ return 8.f; });
	Scrollbar->Express(L"H", [this]{ return Scrollbar->Slimits.Smax.H ; });
	Scrollbar->Scrolled += ThisHandler(OnScrolled);
	AddNode(Scrollbar);

	Body = new CGridBody(l, s, Header->Columns, L"body");
	Body->Express(L"W", [this]{ return Body->Slimits.Smax.W ; });
	Body->Express(L"H", [this]{ return Body->Slimits.Smax.H ; });
	Body->Transform(0, 0, Z_STEP);
	Body->Express(L"C",	[this](auto apply){ return CSize::Empty; });
	AddNode(Body);

	UseClipping(EClipping::Inherit, true);

	Express(L"C",	[this](auto apply)
					{
						auto l = Climits;

						Header->UpdateLayout(Climits, apply);
						
						auto s = l.Smax;
						s.H -= Header->H;
						Scrollbar->UpdateLayout({s, l.Pmax}, apply);
	
						s.W -= Scrollbar->W;
						Body->UpdateLayout({s, l.Pmax}, apply);
	
						float ws = max(0, Spacing.x * (Header->Columns.size() - 1));
						float hs = max(0, Spacing.y * (Rows.size() - 1));
		
						for(auto r : Rows)
						{
							for(auto c : r->Cells)
							{
								///if(c->NeedUpdate)
								///{
								///	c->Current->UpdateLayout();
								///	c->NeedUpdate = false;
								///}
	
								//c->Column->W	= max(c->Column->W, c->Viewer->Size.W);
								c->Row->H		= max(c->Row->H, c->Viewer->Size.H);
							}
						}

						auto w = Header->Columns.Sum<float>([](auto i){ return i->W; }) + ws;
						auto h = Rows.Sum<float>([](auto i){ return i->H; }) + hs;

						return CSize(w + Scrollbar->W, h + Header->H, 0);
					});
}

CGrid::~CGrid()
{
	for(auto i : Rows)
	{
		delete i;
	}

	//RemoveNode(Header);
	Header->Free();
	Scrollbar->Free();
	Body->Free();
}

CGridRow * CGrid::AddRow()
{
	auto r = new CGridRow(Header->Columns);
	Rows.push_back(r);
	return r;
}

CGridColumn * CGrid::AddColumn(CString const & name, float w, EOrder o)
{
	auto c = new CGridColumn(Level, Style);
	c->Index = (int)Header->Columns.size();
	c->Name = name;
	c->W = w;
	c->Order = o;
	Header->Columns.push_back(c);
	return c;
}

void CGrid::Arrange(bool sort)
{
	if(sort)
	{
		auto oc = Header->Columns.Find([](auto i){ return i->Order != EOrder::Null; });
	
		if(oc)
		{
			Rows.Sort(	[oc](CGridRow * a, CGridRow * b)
						{
							auto r = a->Cells[oc->Index]->Entity->Compare(b->Cells[oc->Index]->Entity);
							return oc->Order == EOrder::Up ? r < 0 : r > 0;
						});
		}
	}
	
	float x = 0;
	float y = Body->IH + Scrollbar->Value;

	for(auto r : Rows)
	{
		if(0 < y && y < Body->IH + r->H)
		{
			for(auto c : r->Cells)
			{
				if(!c->Current->Parent)
					Body->AddNode(c->Current);

				c->Current->Transform(x, y - c->Row->H, Z_STEP);
				x += Spacing.x + c->Column->W;
			}
		}
		else
		{
			for(auto c : r->Cells)
			{
				if(c->Current->Parent)
					Body->RemoveNode(c->Current);
			}
		}

		x = 0;
		y -= Spacing.y + r->H;
	}

}

void CGrid::UpdateLayout(CLimits const & l, bool apply)
{
	__super::UpdateLayout(l, apply);
	
	if(apply)
	{
		Header->Transform(0, IH - Header->H, Z_STEP);
		Scrollbar->Transform(IW - Scrollbar->W, 0, Z_STEP);	
		
		auto hs = max(0, Spacing.y * (Rows.size() - 1));
		auto h = Rows.Sum<float>([](auto i){ return i->H; }) + hs;
	
		Scrollbar->SetTotal(h + hs);
		Scrollbar->SetVisible(Body->IH);
	
		Arrange(true);
	}
}

void CGrid::SetSpacing(CFloat2 & s)
{
	Spacing = Header->Spacing = s;
}

void CGrid::OnScrolled()
{
	Arrange(false);
}
#include "StdAfx.h"
#include "Listbox.h"

using namespace uos;

CListbox::CListbox(CWorldLevel * l, CStyle * s, CString const & name) : CRectangle(l, name)
{
	Scroll.Set(0, 0);

	UseClipping(EClipping::Inherit, true);

	Active->Listen(true);
	Active->MouseInput += ThisHandler(OnMouse);

	Express(L"C",	[this](auto apply)
					{
						CSize c;

						for(auto i : Items)
						{
							i->UpdateLayout(Climits, apply);
						}

						return CalculateSize(Nodes);
					});
}

CListbox::~CListbox()
{
	for(auto i : Items)
	{
		if(Nodes.Contains(i))
			RemoveNode(i);

		i->Free();
	}
}

void CListbox::UpdateLayout(CLimits const & l, bool apply)
{
	__super::UpdateLayout(l, apply);

	if(apply)
	{
		if(Scroll.y < IH)
		{
			Scroll.y = IH;
		}

		Arrange(CAnimation());
	}
}

void CListbox::Arrange(CAnimation & a)
{
	if(!isfinite(IH) || !Scroll.IsReal())
		return;

	float h = Items.Sum<float>(	[](auto i) mutable
								{ 
									return i->Size.H; 
								});

	if(Scroll.y > h && h > IH)
	{
		Scroll.y = IH;
	}

	float y;

	if(h < IH)
		y = (YAlign == EYAlign::Bottom) ? h : IH;
	else
		y = Scroll.y;

	for(auto i : Items)
	{
		y -= i->Size.H;

		if(-i->H <= y && y < IH)
		{
			if(!i->Parent)
				AddNode(i);

			CAnimated<CTransformation> a(i->Transformation, CTransformation(0.f, y, Z_STEP), a);

			Level->Core->AddJob(this, L"Listbox animation",	[i, a]() mutable  -> bool 
															{	
																if(a.Animation.Running)
																{
																	i->Transform(a.GetNext());
																	return false;
																} 
																else
																{
																	return true;
																}
															});
		}
		else 
		{
			if(i->Parent)
				RemoveNode(i);
		}
	}
}

void CListbox::AddBack(CElement * e)
{
	AddItem(e, Items.end());
}

void CListbox::AddFront(CElement * e)
{
	AddItem(e, Items.begin());
}

void CListbox::AddItem(CElement * e, CList<CElement *>::iterator before)
{
	if(Climits.Smax && Climits.Pmax)
	{
		e->UpdateLayout(Climits, true);
	}

	Items.insert(before, e);
	
	if(IH > 0.f)
		Scroll.y = IH;
	
	Arrange(Animation);

	e->Take();
}

void CListbox::RemoveItem(CElement * e)
{
	Items.Remove(e);
	Arrange(Animation);
	e->Free();
}

void CListbox::OnMouse(CActive *, CActive *, CMouseArgs * a)
{
	if(a->Class == EInputClass::Mouse && a->Control == EControl::Wheel)
	{
		Scroll.y += -IH * 0.2f * a->Input->RotationDelta.y;

		float h = Items.Sum<float>(	[](auto i) mutable
									{ 
										return i->Size.H; 
									});

		if(Scroll.y > h)
		{
			Scroll.y = h;
		}

		if(Scroll.y < IH)
		{
			Scroll.y = IH;
		}

		Arrange(CAnimation());
	}
}

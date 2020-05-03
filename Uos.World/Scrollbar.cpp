#include "StdAfx.h"
#include "Scrollbar.h"

using namespace uos;

CScrollbar::CScrollbar(CWorldLevel * l, CStyle * s, CString const & name) : CElement(l, name)
{
	Style	= s;

	UseCanvas(Visual, null, l->Engine->PipelineFactory->DiffuseTextureShader);

	Active->SetMesh(Visual->Mesh);

	Active->Listen(true);
	Active->MouseInput += ThisHandler(OnMoveInput);
}

CScrollbar::~CScrollbar()
{
}

void CScrollbar::UpdateLayout(CLimits const & l, bool apply)
{
	__super::UpdateLayout(l, apply);

	S = IW;
	T = IH - S - S;

	UpdateCanvas();
}

void CScrollbar::SetTotal(float v)
{
	Total = v;
	Draw();
}

void CScrollbar::SetVisible(float v)
{
	Visible = v;
	Draw();
}

void CScrollbar::ValueToPosition()
{
}

void CScrollbar::Draw()
{
	if(IH > 1 && IW > 1)
	{
		auto c = Texture->BeginDraw();
		c->Clear(CFloat4(0));
	
		auto b = c->CreateSolidBrush(Style->Get<CFloat4>(L"Border/Material"));
	
		c->DrawRectangle(CRect(1, 1,		S-1, S-1), b, 1.f);
		c->DrawRectangle(CRect(1, IH - S,	S-1, S-1), b, 1.f);
		//c->DrawText(Text, Font, CFloat4(1), CRect(0, 0, float(Texture->W), float(Texture->H)), XAlign, YAlign, Wrap);
		
		if(Visible > 0 && Total > 0)
		{
			G = CFloat::Clamp(Visible/Total * T, 3.f, T);
			P = (T - G) - (T - G) * Value/(Total - Visible);
			c->DrawRectangle(CRect(1, S+P+1, S-1, G-2), b, 1.f);
		}
	
		b->Free();
		
		Texture->EndDraw();
	}
}

void CScrollbar::OnMoveInput(CActive * r, CActive * s, CMouseArgs * a)
{
	if(a->Class == EInputClass::Mouse && a->Capture.Message.Control == EControl::LeftButton)
	{
		if(a->Event == EGraphEvent::Captured)
		{
			Point = Active->Transit(a->Pick.Active, a->Pick.Point);
		}
		else if(a->Capture.IsCaptured() && (a->Event == EGraphEvent::Hover || a->Event == EGraphEvent::Roaming))
		{
			auto q = Active->Transit(a->Pick.Active, a->Pick.Point);
			auto d = (q - Point).y;
			
			auto p = CFloat::Clamp(P + d, 0.f, T-G);
			
			Value = (Total - Visible) * (T-G - p)/(T-G);

			if(!isfinite(Value))
			{
				Value = 0;
			}

			Scrolled();

			//Level->Log->ReportDebug(this, L"%f", d);

			Draw();

			Point = q;
		}
	}
}

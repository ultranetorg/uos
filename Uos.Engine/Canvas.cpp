#include "stdafx.h"
#include "Canvas.h"

using namespace uos;

CCanvas::CCanvas(CEngineLevel * l, CDirectSystem * e, CDirectDevice * d, ID3D11Resource * t)
{
	Engine = e;
	Device = d;
	Texture = t;

	Verify(t->QueryInterface(&Surface));
	
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);
	Verify(e->D2D->CreateDxgiSurfaceRenderTarget(Surface, &props, &RenderTarget));


	RenderTarget->QueryInterface(__uuidof(ID2D1GdiInteropRenderTarget), (void**)&GdiRenderTarget); 

	RenderTarget->BeginDraw();
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	RenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);

	DXGI_SURFACE_DESC sd;
	Surface->GetDesc(&sd);
	Size.W = float(sd.Width);
	Size.H = float(sd.Height);
	Size.D = 0;
	   
/*
	IDWriteRenderingParams * drp;
	GraphicEngine->DWrite->CreateRenderingParams(&drp);

	auto ctl = drp->GetClearTypeLevel();

	IDWriteRenderingParams * nrp;
	GraphicEngine->DWrite->CreateCustomRenderingParams(drp->GetGamma(), 
													   drp->GetEnhancedContrast(), 
													   drp->GetClearTypeLevel(),
													   DWRITE_PIXEL_GEOMETRY_FLAT, 
													   DWRITE_RENDERING_MODE_ALIASED,
													   &nrp);

	RenderTarget->SetTextRenderingParams(nrp);*/
}

CCanvas::~CCanvas()
{
	RenderTarget->EndDraw();
	RenderTarget->Release();
	GdiRenderTarget->Release();
	Surface->Release();
}

CSolidColorBrush * CCanvas::CreateSolidBrush(CFloat4 & c)
{
	return new CSolidColorBrush(this, c);
}

void CCanvas::DrawLine(CFloat2 & a, CFloat2 & b, float sw, CSolidColorBrush * brush)
{
	RenderTarget->DrawLine(D2D1::Point2F(a.x, Size.H - a.y), D2D1::Point2F(b.x, Size.H - b.y), brush->Brush, sw, null);
}

void CCanvas::FillRectangle(CRect & r, CSolidColorBrush * brush)
{
	RenderTarget->FillRectangle(ToDx(r), brush->Brush);
}

void CCanvas::DrawRectangle(CRect & r, CSolidColorBrush * brush, float sw)
{
	RenderTarget->DrawRectangle(ToDx(r), brush->Brush, sw);
}

void CCanvas::Clear(CFloat4 & c)
{
	RenderTarget->Clear(D2D1::ColorF(c.x, c.y, c.z, c.w));
}

void CCanvas::DrawText(CString const & t, CFont * f, CSolidColorBrush * b, CRect & r)
{
///	RenderTarget->DrawText(t.data(), t.size(), f->Format, ToDx(r), b->Brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
	DrawText(t, f, b->Color, r, EXAlign::Left, EYAlign::Top, false, false);
}

void CCanvas::DrawText(CString const & t, CFont * f, CFloat4 & c, CRect & r, EXAlign xa, EYAlign ya, bool wrap, bool ells)
{
	ID3D11RenderTargetView * rtv[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	Device->DxContext->OMGetRenderTargets(_countof(rtv), rtv, null);

	HDC dc;
	Verify(GdiRenderTarget->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &dc));

	RECT  rr = r.ToRECT(Size);

	auto fontOld = (HFONT)SelectObject(dc, f->HFont);

	SetMapMode(dc, MM_TEXT);
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(c.x * 255, c.y * 255, c.z * 255));
	//SetTextAlign(dc, TA_TOP);
	//SetBkColor(dc, 0x00000000);

	//auto b = ::CreateSolidBrush(RGB(255, 0, 255));
	//FillRect(dc, &rr, b);
	//DeleteObject(b);
			
	DRAWTEXTPARAMS p;
	p.cbSize		= sizeof(DRAWTEXTPARAMS);
	p.iLeftMargin	= 0;
	p.iRightMargin	= 0;
	p.iTabLength	= TabLength;
	p.uiLengthDrawn = 0;

	//if(Shadow)
	//{
	//	SetTextColor(dc, RGB(32,32,32));
	//	RECT  a = {1, 0, int(w+1), int(h+1)};
	//	DrawTextEx(dc, LPWSTR(Text.data()), Text.size(), &a, GetDrawFlags(), &p);
	//	RECT  b = {0, 1, int(w+1), int(h+1)};
	//	DrawTextEx(dc, LPWSTR(Text.data()), Text.size(), &b, GetDrawFlags(), &p);
	//}

	DrawTextEx(dc, LPWSTR(t.data()), int(t.size()), &rr, f->GetDrawFlags(wrap, ells, xa, ya), &p);

	SelectObject(dc, fontOld);
	Verify(GdiRenderTarget->ReleaseDC(NULL));
	   
	for(auto i : rtv)
		if(i)
			i->Release();
}

CBitmap * CCanvas::CreateBitmap(CImage * i)
{
	auto b = new CBitmap(this, i->Size);
	b->Load(i);
	return b;
}

void CCanvas::DrawBitmap(CBitmap * b, CFloat2 & p)
{
	RenderTarget->DrawBitmap(b->Bitmap, ToDx(p, b->Size));
}

D2D1_RECT_F CCanvas::ToDx(CFloat2 & p, CSize & s)
{
	return D2D1::RectF(p.x, Size.H - (p.y + s.H), p.x + s.W, Size.H - p.y);
}

D2D1_RECT_F CCanvas::ToDx(CRect & r)
{
	return D2D1::RectF(r.X, Size.H - (r.Y + r.H), r.X + r.W, Size.H - r.Y);
}
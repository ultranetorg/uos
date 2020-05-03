#include "StdAfx.h"
#include "PolyText.h"

#if 0
namespace mw
{
	CText::CText(CWorldLevel * l, const CString & v, int c /*= 0xFFFF'FFFF*/, bool wrap /*= true*/, CFont * f)  : CElement(l, L"Text")
	{
		Level		= l;
		Engine			= l->Engine;
		VMesh		= Engine->CreateMesh();
		//AMesh		= null;
		Wrap		= false;
		Font		= null;
	
		Visual->SetMesh(VMesh);
		Visual->PixelAccuracy = true;

		SetFont(f);
		SetWrap(wrap);
		SetColor(c);
		SetText(v);
	}
	
	CText::CText(CWorldLevel * l, const CString & v, int c, bool wrap, const CString & family, int size, bool bold, bool italic) : CText(l, v, c, wrap, l->FontFactory->GetFont(family, size, bold, italic))
	{
	}

	CText::~CText()
	{
		//delete VMesh;
		//TNode->Free();
		//VContent->RemoveNode(VText);
		//VText->Free();
		VMesh->Free();
	}
	
	void CText::UpdateArea(const CArea & max, bool apply)
	{
		__super::UpdateArea(max, apply);
	}
		
	CArea CText::UpdateContentLayout(CArea & imax, bool apply)
	{
		auto e = Font->Measure(Text, imax.Width, imax.Height, Wrap);

		CArea c;
		c.Width = e.x + 1;
		c.Height = e.y;
		c.Depth = 0;

		VMesh->SetBBox(CBBox(CFloat3(0,0,0), CFloat3(c.Width, c.Height, 0)));

		if(apply)
		{
			Apply();
		}

		return c;
	}
	
	void CText::SetArea(CArea & a)
	{
		CElement::SetArea(a);
		Apply();
	}

	void CText::SetFont(CFont * f)
	{
		if(Font != f)
		{
			Font = f;
			Visual->SetMaterial(Font->Material);
			Apply();
		}
	}

	void CText::SetText(const CString & t)
	{
		Text = t;
		NeedApply = true;
	}

	void CText::SetWrap(bool e)
	{
		Wrap = e;
	}

	void CText::SetColor(int c)
	{
		Color = c;
		auto n = VMesh->GetVColors().size();
		VMesh->SetVColors(CArray<int>(n, c));
	}

	//void CText::SetVerticalAlignment(EVerticalAlignment a)
	//{
	//	VAlign = a;
	//}

	void CText::Apply()
	{
		if(Font == null || !isfinite(IW) || !isfinite(IH))
		{
			return;
		}

		float x = 0.f; 
		float y = IH;

		float fStartX = x;

		wchar_t * p = const_cast<wchar_t *>(Text.c_str());

		VMesh->SetPositionFormat(3, false);
		CArray<CFloat3>	v(Text.size()*4);
		CArray<int>		vc(Text.size()*4);
		CArray<CFloat2>	uv(Text.size()*4);
		CArray<int>		ix(Text.size()*6);

		int iv = 0;
		int ii = 0;


		
		while(*p)
		{
			auto m = Font->MeasureLine(p, IW, Wrap);

			if(m.Count == 0) // in case of IW < char_width 
			{
				break;
			}

			while(m.Count > 0)
			{
				wchar_t c = *p++;
				m.Count--;
				
				if(c == L'\n' || c == '\r')
				{	
					continue;
				}

				if(c == L'\t' || c == ' ')
				{	
					x += Font->CharWidths[c];
					continue;
				}

				if(c > Font->TextMetric.tmLastChar || (Font->TexCoords[c].B == 0.f && Font->TexCoords[c].L == 0.f && Font->TexCoords[c].R == 0.f && Font->TexCoords[c].T == 0.f))
				{
					c = Font->TextMetric.tmDefaultChar;
				}

				FLOAT tl = Font->TexCoords[c].L;
				FLOAT tt = Font->TexCoords[c].T;
				FLOAT tr = Font->TexCoords[c].R;
				FLOAT tb = Font->TexCoords[c].B;

				FLOAT w = (tr-tl) * Font->TextureWidth	/ Font->Scale;
				FLOAT h = (tb-tt) * Font->TextureHeight	/ Font->Scale;

				v[iv+0] = CFloat3(x, y,	 0.f);		v[iv+1] = CFloat3(x + w, y,	 0.f);
				v[iv+2] = CFloat3(x, y - h, 0.f);	v[iv+3] = CFloat3(x + w, y - h, 0.f);

				uv[iv+0] = CFloat2(tl, tt);	uv[iv+1] = CFloat2(tr, tt);
				uv[iv+2] = CFloat2(tl, tb);	uv[iv+3] = CFloat2(tr, tb);

				vc[iv+0] = Color;	vc[iv+1] = Color;
				vc[iv+2] = Color;	vc[iv+3] = Color;

				ix[ii+0] = iv+0;
				ix[ii+1] = iv+1;
				ix[ii+2] = iv+2;
				ix[ii+3] = iv+2;
				ix[ii+4] = iv+1;
				ix[ii+5] = iv+3;

				iv += 4;
				ii += 6;

				x += w;
			}

			if(!Wrap)
			{
				break;
			}

			x =	fStartX;
			y -= Font->Height;
		}

		//v.resize(iv);
		//vc.resize(iv);
		//uv.resize(iv);
		//ix.resize(ii);

		VMesh->SetPositions(v);
		VMesh->SetVColors(vc);
		VMesh->SetUVs(uv);
		VMesh->SetIndexes(ix);
		VMesh->SetPrimitiveInfo(D3DPT_TRIANGLELIST, ii/3);

	//	NeedApply  = false;
	}

}
#endif
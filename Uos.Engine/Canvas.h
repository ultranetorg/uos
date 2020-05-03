#pragma once
#include "DirectSystem.h"
#include "Image.h"
#include "Font.h"

namespace uos
{
	class CSolidColorBrush;
	class CCanvasFont;
	class CBitmap;

	class UOS_ENGINE_LINKING CCanvas
	{
		public:
			CEngineLevel *								Level;
			CDirectDevice *								Device;
			CDirectSystem *								Engine;
			ID3D11Resource *							Texture;
			IDXGISurface1 *								Surface = null;
			ID2D1RenderTarget *							RenderTarget = null;
			CSize										Size;
			int											TabLength = 4;
			ID2D1GdiInteropRenderTarget *				GdiRenderTarget;

			CCanvas(CEngineLevel * l, CDirectSystem * e, CDirectDevice * d, ID3D11Resource * t);
			~CCanvas();

			CSolidColorBrush *							CreateSolidBrush(CFloat4 & c);
			CBitmap *									CreateBitmap(CImage * i);
			void										DrawText(CString const & t, CFont * f, CSolidColorBrush * b, CRect & r);
			void										DrawText(CString const & t, CFont * f, CFloat4 & c, CRect & r, EXAlign xa, EYAlign ya, bool wrap, bool ells);
			//UINT										GetDrawFlags(EXAlign xa, EYAlign ya, bool wrap);
			//CRect										MeasureText(CString const & t, CFont * f, float maxw, float maxh);
			void										DrawLine(CFloat2 & a, CFloat2 & b, float sw, CSolidColorBrush * brush);
			void										FillRectangle(CRect & r, CSolidColorBrush * brush);
			void										DrawRectangle(CRect & r, CSolidColorBrush * brush, float sw);
			void										Clear(CFloat4 & c);
			void										DrawBitmap(CBitmap * b, CFloat2 & p);

			D2D1_RECT_F									ToDx(CFloat2 & p, CSize & s);
			D2D1_RECT_F									ToDx(CRect & r);
	};

	class UOS_ENGINE_LINKING CSolidColorBrush : public virtual CShared
	{
		public:
			ID2D1SolidColorBrush *	Brush;
			CFloat4					Color;

			CSolidColorBrush(CCanvas * cs, CFloat4 & c)
			{
				Color = c;
				cs->RenderTarget->CreateSolidColorBrush(D2D1::ColorF(c.x, c.y, c.z, c.w), &Brush);
			}

			~CSolidColorBrush()
			{
				Brush->Release();
			}
	};

	class UOS_ENGINE_LINKING CBitmap 
	{
		public:
			ID2D1Bitmap *	Bitmap;
			CCanvas *		Canvas;
			CSize			Size;

			CBitmap(CCanvas * cs, CSize & s)
			{
				Canvas = cs;
				Size = s;

				D2D1_BITMAP_PROPERTIES p;
				p.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
				p.pixelFormat.alphaMode = D2D1_ALPHA_MODE_STRAIGHT;
				p.dpiX = 96;
				p.dpiY = 96;
				Canvas->RenderTarget->CreateBitmap(D2D1_SIZE_U{UINT32(s.W), UINT32(s.H)}, p, &Bitmap);
			}

			~CBitmap()
			{
				Bitmap->Release();
			}

			void Load(CImage * i)
			{
				Bitmap->CopyFromMemory(&D2D1_RECT_U{0, 0, UINT32(i->Size.W), UINT32(i->Size.H)}, i->Pixels.GetData(), UINT32(i->Size.W * i->BytesPerPixel));
			}
	};

}
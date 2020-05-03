#pragma once
#include "Float2.h"
#include "Float.h"
#include "GdiRect.h"
#include "Array.h"
#include "Buffer.h"
#include "Exception.h"
#include "ISerializable.h"
#include "Size.h"

namespace uos
{
	class UOS_LINKING CRect : public ISerializable
	{
		public:
			float		X;
			float		Y;
			float		W;
			float		H;

			CRect();
			CRect(std::initializer_list<float> l);
			CRect(CFloat2 & p, CFloat2 & s);
			CRect(float x, float y, float w, float h);
			CRect(const CGdiRect & r, const CFloat2 o);
			CRect(const RECT & r, const CFloat2 o);

			std::wstring								GetTypeName();

			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & v);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;

			RECT										ToRECT(CSize & s);
			void										Set(float x, float y, float w, float h);
			void										MakeEmpty();
			CRect										GetLocal();
			CFloat2										GetCenter();
			bool										IsEmpty();
			CRect										GetInflated(float dx, float dy);
			bool										IsGreaterOrEqual(float w, float h);
			bool										IsIntersectWOB(const CRect & a);
			
			bool										Intersects(const CRect & a);
			CRect										Intersect(const CRect & a);
			void										SetPosition(CFloat2 & v);
			float										GetArea();
			float										GetWidth();
			float										GetHeight();
			CFloat2										GetLT();
			CFloat2										GetLB();
			CFloat2										GetRT();
			CFloat2										GetRB();
			float										GetRight() const;
			float										GetBottom() const;
			float										GetTop() const;
			float										GetLeft() const;
			CFloat2										GetPoint(float kw, float kh);
			void										Bound(CFloat2 & v);
			bool										Contains(float x, float y);
			bool										ContainGDI(float x, float y);
			bool										Contains(CFloat2 & p);
			bool										ContainNoBorder(float x, float y);
			bool										operator == (const CRect & a) const;
			bool										operator != (const CRect & a) const;
			CRect										operator + (const CRect & a);
	};
}
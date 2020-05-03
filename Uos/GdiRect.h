#pragma once
#include "Float2.h"
#include "Converter.h"
#include "Int32.h"

namespace uos
{
	class UOS_LINKING CGdiRect : public ISerializable
	{
		public:
			int	X;
			int	Y;
			int	Width;
			int	Height;

			const static std::wstring					TypeName;

			CGdiRect();
			CGdiRect(int x, int y, int w, int h);
			CGdiRect(const RECT & r);
			CGdiRect(const CBuffer & b);

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & v);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;

			void										Set(int x, int y, int w, int h);
			void										MakeEmpty();
			CGdiRect									GetLocal();
			POINT										GetCenter();
			bool										IsEmpty();
			CGdiRect									GetInflated(int dx, int dy);
			bool										IsGreaterOrEqual(int w, int h);
			bool										IsIntersectWOB(const CGdiRect & a);
			bool										IsIntersect(const CGdiRect & a);
			CGdiRect									Intersect(const CGdiRect & a);
		
			int											GetArea();
			int											GetWidth();
			int											GetHeight();
			int											GetRight() const;
			int											GetButtom() const;
			bool										Contain(int x, int y);
			bool										ContainWOB(int x, int y);
			RECT										GetAsRECT();

			bool operator ==							(const CGdiRect & a) const;
			bool operator !=							(const CGdiRect & a) const;
			CGdiRect operator +							(const CGdiRect & a);

			CString ToString();
	};
}
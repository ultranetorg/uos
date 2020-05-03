#pragma once
#include "Math.h"
#include "String.h"
#include "Float.h"

namespace uos
{
	class UOS_LINKING CFloat2 : public ISerializable
	{
		public:
			float x, y;

			const static std::wstring					TypeName;
			static const CFloat2 						Nan;

			CFloat2(){}
			CFloat2(float a) : x(a), y(a){}
			CFloat2(float ax, float ay) : x(ax), y(ay){}
			CFloat2(POINT & p) : x((float)p.x), y((float)p.y){}
			CFloat2(CString const & name) { Read(name); }
			~CFloat2(){}

			CFloat2 operator +							(const CFloat2&) const;
			CFloat2 operator -							(const CFloat2&) const;
			CFloat2 operator *							(const CFloat2 & p) const;
			CFloat2	operator *							(float) const;
			bool	operator ==							(const CFloat2&) const;
			bool	operator !=							(const CFloat2&) const;

			CFloat2 operator =							(const CFloat2& a) { x = a.x; y = a.y; return *this; }

			operator									D2D1_POINT_2F();
			operator									DirectX::XMFLOAT2 * ();
			operator									DirectX::XMFLOAT2 * () const;
			operator									bool();
			bool operator <								(CFloat2 const & a);


			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;
			
			CString										ToString();
			void										Set(float px, float py);
			float										GetLength();
			float										GetLengthSq();
			bool										IsReal();
			float										Dot(CFloat2 & v);
			bool static									SegmentIntersection(CFloat2 start1, CFloat2 end1, CFloat2 start2, CFloat2 end2, CFloat2 *out_intersection);
			bool static									LineSegmentIntersection(CFloat2 la, CFloat2 lb, CFloat2 sstart, CFloat2 send, CFloat2 *out_intersection);
			bool static									ProjectPointOntoSegment(CFloat2 & p, CFloat2 & v, CFloat2 & w, CFloat2 * pj, float * d);

			CFloat2										Cross(CFloat2 & v);
			static float								PointToRayDistance(CFloat2 & o, CFloat2 & d, CFloat2 & p); // o - origin of 'this' line;


			float GetDistance(CFloat2 & v);
	};
}
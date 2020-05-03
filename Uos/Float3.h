#pragma once
#include "Float2.h"
#include "Angle.h"
#include "String.h"

namespace uos
{
	class CMatrix;

	class UOS_LINKING CFloat3 : public ISerializable
	{
		public:
			float x, y, z;

			static const std::wstring					TypeName;
			static const CFloat3 						Nan;

			CFloat3();
			CFloat3(std::initializer_list<float> l);
			CFloat3(float xyz);
			CFloat3(float x, float y, float z);
			CFloat3(const CString & v);
			~CFloat3();

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;

			CString										ToString();

			bool										IsReal() const;
			void										Set(float px, float py, float pz);
			CFloat3										Cross(CFloat3 & v);
			float										Dot(CFloat3 & v);
			//void										Normalize();
			CFloat3										GetNormalized();
			float										Length();
			float										SquaredLength();
			void										SetLength(float l);
			float										GetLengthOfProjectionXZ();
			float										GetAngleInXZ(); // XZ projection of vector and Z
			float										GetAngleInYZ(); // YZ projection of vector and Z
			float										GetAngleInXY(); // XY projection of vector and X
			float										GetAngleInXZ(CFloat3 & a);
			float										GetAngleInYZ(CFloat3 & a);
			float										GetAngleInXY(CFloat3 & a);
			float										GetAngle(CFloat3 & a);
			float										GetAngleToXY(); // vector and projection in XY
			float										GetAngleToXZ(); // vector and projection in XZ
			float										GetAngleToYZ(); // vector and projection in YZ

			CFloat3										ReplaceX(float x);
			CFloat3										ReplaceY(float y);
			CFloat3										ReplaceZ(float z);

			float										GetDistance(CFloat3 & v);
			float										GetDistanceToPoint(CFloat3 & o, CFloat3 & p);
			static float								LineToLineDistance(CFloat3 & A, CFloat3 & B, CFloat3 & C, CFloat3 & D, CFloat3 & Pos1, CFloat3 & Pos2);
			static float								RayToRayDistance(CFloat3 & A, CFloat3 & p1, CFloat3 & C, CFloat3 & p2, CFloat3 & Pos1, CFloat3 & Pos2);
			static float								RayToPointDistance(CFloat3 & o, CFloat3 & d, CFloat3 & p);
		
			CFloat3										VertexTransform(CMatrix & m);
			CFloat3										NormalTransform(CMatrix & v);
		
			CFloat3	operator +							(const float) const;
			CFloat3	operator -							(const float) const;
			CFloat3	operator *							(const float) const;
			CFloat3	operator /							(const float) const;

			CFloat3	operator +							(const CFloat3&) const;
			CFloat3	operator -							(const CFloat3&) const;
			CFloat3	operator *							(const CFloat3&) const;
			CFloat3	operator /							(const CFloat3&) const;

			bool operator !=							(const CFloat3&) const;
			bool operator ==							(const CFloat3&) const;

			CFloat3	operator *							(const CMatrix &) const; // trasform coord
			
			CFloat2 									ToXY();
			CFloat2										ToXZ();

			void										AdjustRotationTo2Pi();
			void										AdjustRotationToPiRange();
			
			CString										ToNiceString();

			operator									DirectX::XMFLOAT3 * ();
			operator									DirectX::XMFLOAT3 * () const;
			operator									bool();
			bool operator <								(CFloat3 const & a);
	};


	class UOS_LINKING CRay
	{
		public:
			CFloat3										Origin;
			CFloat3										Direction;

			CRay										Transform(const CMatrix& m);
			bool										IsReal();

			operator bool()
			{
				return IsReal();
			}

			CRay(){}
			CRay(CFloat3 const &  o, CFloat3 const & d) : Origin(o), Direction(d){}
	};

}

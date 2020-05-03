#pragma once
#include "Quaternion.h"
#include "Exception.h"

namespace uos
{
	class UOS_LINKING CTransformation : public ISerializable
	{
		public:
			const static std::wstring					TypeName;
			const static CTransformation				Nan;
			const static CTransformation				Identity;

			CFloat3										Position;
			CQuaternion									Rotation;
			CFloat3										Scale;
			
			CTransformation();
			CTransformation(const CFloat3 & p);
			CTransformation(CFloat3 & p, CQuaternion & r);
			CTransformation(CFloat3 & p, CQuaternion & r, float s);
			CTransformation(CFloat3 & p, CQuaternion & r, CFloat3 & s);
			CTransformation(float px, float py, float pz);
			CTransformation(float px, float py, float pz, float rx, float ry, float rz, float rw);
			CTransformation(float px, float py, float pz, float rx, float ry, float rz, float rw, float sx, float sy, float sz);
			~CTransformation();
			
			CTransformation operator + 					(const CTransformation & a);
			CTransformation operator - 					(const CTransformation & a);
			CTransformation operator * 					(CTransformation & a);
			CTransformation operator * 					(const float & a);

			bool operator ==							(const CTransformation & a);
			bool operator !=							(const CTransformation & a);

			static CTransformation						FromPosition(CFloat3 & p);
			static CTransformation						FromPosition(float x, float y, float z);
			static CTransformation						FromRotation(CQuaternion & r);
			static CTransformation						FromRotation(float x, float y, float z, float w);
			static CTransformation						FromScale(CFloat3 & s);
			static CTransformation						FromScale(float x, float y, float z);

			CTransformation								GetInversed();
			bool										IsReal() const;
			CString										ToStringPRS();

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & v);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;

	};

/*
	class PLATFORM_PRIMARY_CLASS CAnimatedTransformation : public CAnimated<CTransformation>
	{
		public:
			void										StartAnimating(const CTransformation & v, float time);
	};
*/

}
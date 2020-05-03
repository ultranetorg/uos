#pragma once
#include "Transformation.h"
#include "Exception.h"

namespace uos
{
	class CFloat4;
	class CFloat3;

	class UOS_LINKING CMatrix : public ISerializable
	{
		public:
			union {
				struct {
					float        _11, _12, _13, _14;
					float        _21, _22, _23, _24;
					float        _31, _32, _33, _34;
					float        _41, _42, _43, _44;

				};
				float m[4][4];
			};



			const static std::wstring					TypeName;
			const static CMatrix						Nan;
			const static CMatrix						Identity;

			CMatrix(float x, float y, float z);
			CMatrix(float x, float y, float z, float w);
			CMatrix(float s);

			CMatrix(const CQuaternion & q);
			CMatrix(const CTransformation & t);
			CMatrix(CFloat3 & t, CQuaternion & r);
			CMatrix(const ISerializable & a);
			CMatrix(std::initializer_list<float> l);
			CMatrix();
			~CMatrix();

			operator DirectX::XMFLOAT4X4 * ()
			{
				return (DirectX::XMFLOAT4X4 *)m;
			}

			operator DirectX::XMFLOAT4X4 * () const
			{
				return (DirectX::XMFLOAT4X4 *)m;
			}

			CMatrix operator ! ()
			{
				return GetInversed();
			}

			void operator =								(const ISerializable & a);
			bool operator !=							(const CMatrix & a);

			std::wstring								GetTypeName();

			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & v);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;
			
			CMatrix static								FromPosition(float x, float y, float z);
			CMatrix static								FromPosition(const CFloat3 & p);
			CMatrix static								FromScaling(float x, float y, float z);
			CMatrix static								FromScaling(const CFloat3 & s);
			CMatrix static								FromRotationAxis(CFloat3 & ax, float angle);
			CMatrix static								FromEuler(float x, float y, float z);
			CMatrix static								FromEuler(CFloat3 & ypr);
			CMatrix static								FromPerspectiveFovLH(float yfov, float aspect, float znear, float zfar);
			CMatrix static								FromOrthographicOffCenterLH(float l, float r, float b, float t, float znear, float zfar);
			CMatrix static								FromLookToLH(CFloat3 & eye, CFloat3 & dir, CFloat3 & up);
			CMatrix static								FromTransformation(CFloat3 const & t, CQuaternion const & r, CFloat3 const & rc, CFloat3 const & s);

			bool										IsReal();

			CMatrix										GetInversed() const;
			CMatrix										GetTransposed();
			CTransformation								Decompose();
			CQuaternion									ToQuaternion();

			//void										MakeTranslation(float x, float y, float z);
			//void										MakeScaling(float sx, float sy, float sz);
			//void										MakeRotationYawPitchRoll(CFloat3 & r);
			//void										MakeRotationX(float angle);
			//void										MakeRotationY(float angle);
			//void										MakeRotationZ(float angle);
			//void										MakeAffineTransformation(CFloat3 & t, CFloat3 & r, CFloat3 & rc, CFloat3 & s);
			CFloat3										TransformCoord(CFloat3 & v) const;
			CFloat3										TransformNormal(CFloat3 & v) const;

			CFloat3										GetPosition();
//			CFloat3										GetRotation();

			CMatrix operator *							(const CMatrix&) const;
			CFloat4 operator *							(const CFloat4& v);


			void operator *=							(const CMatrix& v);

	};
}
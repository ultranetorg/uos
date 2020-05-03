#pragma once
#include "Float3.h"
#include "Converter.h"

namespace uos
{
	class UOS_LINKING CQuaternion
	{
		public:
			float x, y, z, w;

			CQuaternion(){}
			CQuaternion(std::initializer_list<float> && l);
			CQuaternion(float x, float y, float z, float w);
			CQuaternion(float p, float y, float r);
			CQuaternion(CFloat3 & r);
			CQuaternion(CFloat3 & a, CFloat3 &b);
			~CQuaternion(){}

			CFloat3										ToEuler();
			CQuaternion									GetInversed();
			CQuaternion									Normalize();
			bool										IsIdentity();
			
			CQuaternion operator * 						(const CQuaternion & a);
			CQuaternion operator * 						(const float & a);
			CQuaternion operator +						(const CQuaternion & a);
			CQuaternion operator -						(const CQuaternion & a);

			CString										ToString();
			CString										ToNiceString();
			bool										IsReal() const;

			void										Read(CStream * s);
			int64_t										Write(CStream * s);

			CQuaternion									FromRotationAxis(CFloat3 & ax, float angle);

			operator DirectX::XMFLOAT4 * ()
			{
				return (DirectX::XMFLOAT4 *)&x;
			}

			operator DirectX::XMFLOAT4 * () const
			{
				return (DirectX::XMFLOAT4 *)&x;
			}
	};
}
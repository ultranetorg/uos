#pragma once
#include "Float3.h"

namespace uos
{
	class UOS_LINKING CFloat4 : public ISerializable
	{
		public:
			float x, y, z, w;
			
			const static std::wstring					TypeName;
			static const CFloat4 						Nan;

			CFloat4(float xyzw);
			CFloat4(float x, float y, float z, float w);
			CFloat4(CFloat3 & v, float w);
			CFloat4();
			~CFloat4();

			operator DirectX::XMFLOAT4 * ()
			{
				return (DirectX::XMFLOAT4 *)(&x);
			}

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;

			CString										ToString();
			
			CFloat3 &									GetXYZ();
			CFloat2 &									GetXY();
		
			bool										IsReal();

			static CFloat4								Rand(float v);
			void										Normalize();
			float										Dot(CFloat4 & v);
	};
}
#pragma once
#include "Converter.h"
#include "Exception.h"

namespace uos
{
	class UOS_LINKING CFloat : public ISerializable
	{
		public:
			const static float							PI;
			const static std::wstring					TypeName;

			CFloat(){}
			CFloat(float v) : Value(v){}
			CFloat(CString const & name) { Read(name); }

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;

			float										Value;

			static float 								Parse(const CString & s);

			static CString NiceFormat(float x);

			operator float()
			{
				return Value;
			}

			template<typename T> static T Clamp(T v, T vmin, T vmax)
			{
				return min(vmax, max(vmin, v));
			}

	};
}

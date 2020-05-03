#pragma once
#include "Exception.h"
#include "String.h"

namespace uos
{
	class UOS_LINKING CInt64 : public ISerializable
	{
		public:
			int64_t										Value;
			const static std::wstring					TypeName;

			CInt64(){}
			CInt64(int64_t v) : Value(v){}
			CInt64(CString const & v) { Read(v); }

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;
			
			static CString 								ToString(int64_t i);
			static int64_t								Parse(const std::wstring & b);
			static int64_t								Parse(const std::wstring & s, const std::wstring & f);
			static bool									Valid(const CString& s);

			operator int64_t()
			{
				return Value;
			}
	};
}

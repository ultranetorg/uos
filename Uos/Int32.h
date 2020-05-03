#pragma once
#include "Converter.h"
#include "Exception.h"

namespace uos
{
	class UOS_LINKING CInt32 : public ISerializable
	{
		public:
			int											Value;
			const static std::wstring					TypeName;

			CInt32(){}
			CInt32(int v) : Value(v){}
			CInt32(CString const & v) { Read(v); }

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;


			static CString 								ToString(int i);
			static int									Parse(const std::wstring & b);
			static bool Valid(const CString& s);

			operator int()
			{
				return Value;
			}
	};
}

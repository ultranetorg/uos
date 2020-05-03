#pragma once
#include "ISerializable.h"
#include "Exception.h"

namespace uos
{	
	class UOS_LINKING CBool : public ISerializable
	{
		public:
			const static std::wstring					TypeName;

			CBool(){}
			CBool(bool v) : Value(v){}

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;

			bool										Value;

			operator bool ()
			{
				return Value;
			}
	};
}

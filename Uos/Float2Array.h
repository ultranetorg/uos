#pragma once
#include "ISerializable.h"
#include "Converter.h"
#include "Exception.h"
#include "Float2.h"

namespace uos
{	
	class UOS_LINKING CFloat2Array : public CArray<CFloat2>, public ISerializable
	{
		public:
			const static std::wstring					TypeName;

			CFloat2Array(){}
			CFloat2Array(const CArray<CFloat2> & v) : CArray<CFloat2>(v){}

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;
	};
}

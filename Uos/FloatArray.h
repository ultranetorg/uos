#pragma once
#include "ISerializable.h"
#include "Converter.h"
#include "Exception.h"

namespace uos
{	
	class UOS_LINKING CFloatArray : public CArray<float>, public ISerializable
	{
		public:
			const static std::wstring					TypeName;

			CFloatArray(){}
			CFloatArray(CArray<float> & v) : CArray<float>(v){}
			CFloatArray(std::initializer_list<float> v) : CArray<float>(v){}

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;
	};
}

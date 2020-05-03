#pragma once
#include "Converter.h"
#include "Base64.h"

namespace uos
{
	class UOS_LINKING CSerializableBuffer : public CBuffer, public ISerializable
	{
		public:
			const static std::wstring					TypeName;

			std::wstring								GetTypeName();

			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;


			CSerializableBuffer() : CBuffer() {}
			CSerializableBuffer(const CBuffer & b) : CBuffer(b){}
			CSerializableBuffer(int64_t size) : CBuffer(size){}
			CSerializableBuffer(const void * data, int64_t size) : CBuffer(data, size){}
	};
}

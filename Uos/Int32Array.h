#pragma once
#include "ISerializable.h"
#include "Converter.h"
#include "Exception.h"

namespace uos
{	
	class CInt32Array : public CArray<int>, public ISerializable
	{
		public:
			UOS_LINKING const static std::wstring	TypeName;

			UOS_LINKING CInt32Array(){}
			UOS_LINKING CInt32Array(const CArray<int> & v) : CArray<int>(v){}
			
			UOS_LINKING std::wstring				GetTypeName();
			UOS_LINKING void						Read(CStream * s);
			UOS_LINKING int64_t					Write(CStream * s);
			UOS_LINKING void						Write(std::wstring & s);
			UOS_LINKING void						Read(const std::wstring & b);
			UOS_LINKING ISerializable *			Clone();
			UOS_LINKING bool						Equals(const ISerializable & a) const override;
	};
}

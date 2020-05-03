#pragma once
#include "ISerializable.h"
#include "Converter.h"
#include "Exception.h"
#include "Float3.h"

namespace uos
{	
	class UOS_LINKING CFloat3Array : public CArray<CFloat3>, public ISerializable
	{
		public:
			const static std::wstring					TypeName;

			CFloat3Array(){}
			CFloat3Array(const CArray<CFloat3> & v) : CArray<CFloat3>(v){}

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;



	};
/*
	class UOS_FRAMEWORK_CLASS CVector3ArrayParameter : public CParameter
	{
		public:
			void										Load(CTextNode * m);
			void										Load(CStream * cvalues, int csize);

			void										SetValue(CArray<CFloat3> & v);
			CArray<CFloat3> &							GetValue();
			CArray<CFloat3> &							GetDefault();
			bool										Validate(const CString & v);
			void										Serialize(CString & m);
			void										Serialize(CStream * s);
			int											GetBinarySerializationSize();
			CParameter *								Clone(CParameter * parent, EParameterState::Type state);
			void										Commit();

			CVector3ArrayParameter(const CString & name, EParameterState::Type state, CParameter * p) : CParameter(name, EParameterType_ArrayVector3Float32, state, p) {}
		
		private:
			CArray<CFloat3>							Value;
	};*/
		
}

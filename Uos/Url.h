#pragma once
#include "NativePath.h"
#include "ISerializable.h"

namespace uos
{
	class UOS_LINKING CUrl : public ISerializable
	{
		public:
			CString										Protocol;
			CString										Domain;
			CString										Path;
			CMap<CString, CString>						Query; // TreeMap only !!!!!!!!!

			static const CUrl &							Empty;					
			static const std::wstring					TypeName;

			CUrl(){}
			CUrl(const CUrl & l, const CString & n, const CString & v);
			CUrl(const CString & protocol, const CString & path);
			CUrl(const CString & addr);

			bool										operator == (const CUrl & a) const;
			bool										operator != (const CUrl & a);
			bool										operator < (const CUrl & a) const;
			CString &									operator[] (const CString & p);

			CString										ToString() const;
			bool										PathEquals(CUrl & u);
			bool										IsEmpty();
			CString										GetParamOrEmpty(const CString & p);
			
			static CString								EncodeParameter(const CString &value);
			static CString								DecodeParameter(const CString & src);
			//CString ToName();

			static void									Read(const std::wstring & b, CString * pr, CString * domain, CString * path, CMap<CString, CString> * query);

			std::wstring								GetTypeName() override;
			void										Write(std::wstring &) override;
			void										Read(CStream * s) override;
			int64_t										Write(CStream * s) override;
			void										Read(const std::wstring & b) override;
			ISerializable *								Clone() override;
			bool										Equals(const ISerializable & a) const override;

			static CString								Join(CString const & a, CString const & b);
	};
	

}

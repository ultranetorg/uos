#pragma once
#include "Url.h"

namespace uos
{
	class UOS_LINKING CUsl : public ISerializable
	{
		public:
			const static CString						Protocol;
			CString										Domain;
			CString										Server;

			const static std::wstring					TypeName;

			CUsl();
			CUsl(const CUrl & u);
			CUsl(CString const & u);
			CUsl(CString const & d, CString const & s);
			
			bool										operator==(const CUsl & a) const;
			bool										operator!=(const CUsl & a) const;
			virtual CString								ToString() const;
			bool										IsEmpty();
			bool static									IsUsl(const CUrl & u);

			operator									CUrl() const;

			/// ISerializable

			virtual std::wstring						GetTypeName() override;
			virtual void								Read(CStream * s)  override;
			virtual int64_t								Write(CStream * s) override;
			virtual void								Write(std::wstring & s) override;
			virtual void								Read(const std::wstring & addr) override;
			virtual ISerializable *						Clone() override;
			virtual bool								Equals(const ISerializable & a) const override;

	};


	class UOS_LINKING CUol : public CUsl // universal object locator
	{
		public:
			CString										Object;
			CMap<CString, CString>						Parameters;

			const static std::wstring					TypeName;

			CUol();
			CUol(const CUrl & addr);
			CUol(CUsl & u, CString const & o);
			CUol(CUsl & u, CString const & o, CMap<CString, CString> const & params);
			CUol(CUrl & u, CString const & o);
			CUol(CString const & d, CString const & s, CString const & o);

			static CString								GetObjectType(CString const & name);
			static CString								GetObjectID(CString const & name);

			bool static									IsValid(const CUrl & u);
			bool										IsEmpty() const;
			CString										GetType();
			CString										GetId() const;
			virtual CString								ToString() const override;
			
			bool										operator!= (const CUol & a) const;
			bool										operator== (const CUol & a) const;
			CUol &										operator=  (CUrl & addr);
			operator									CUrl() const;
			
			/// ISerializable

			virtual std::wstring						GetTypeName() override;
			virtual void								Read(CStream * s) override { __super::Read(s); };
			virtual void								Read(const std::wstring & addr) override;
			virtual ISerializable *						Clone() override;
			virtual bool								Equals(const ISerializable & a) const override;
	};

	class UOS_LINKING CUrq : public CUrl // universal system request
	{
		public:
			CUrq(CString const & u);
			CUrq(CUol const & u);
			CUrq(CUrl const & u);

			CString										GetSystem() const;
			CString										GetObject() const; 
	};

}

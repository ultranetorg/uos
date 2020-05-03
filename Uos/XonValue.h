#pragma once
#include "String.h"
#include "MemoryStream.h"
#include "IType.h"

namespace uos
{
	class CXonValue : public IType 
	{
		public:
			UOS_RTTI
			virtual void		Set(CXonValue * v)=0;
			virtual bool		Equals(const CXonValue &) const = 0;
			virtual CXonValue * Clone() = 0;
	};

	class CXonSimpleValue : public CXonValue
	{
		public:
			virtual void	Set(const ISerializable & s)=0;
			virtual void	Get(ISerializable & s)=0;
	};

	class CXonTextValue : public CXonSimpleValue
	{
		public:
			CString	Text;

			void Set(CXonValue * v) override
			{
				Text = dynamic_cast<CXonTextValue *>(v)->Text;
			}
			
			void Set(const ISerializable & s) override
			{
				Text.clear();
				((ISerializable &)s).Write(Text);
			}
			
			void Get(ISerializable & s) override
			{
				s.Read(Text);
			}
			
			bool Equals(const CXonValue & v) const override
			{
				return Text == ((CXonTextValue &)v).Text;
			}
			
			CXonValue * Clone() override
			{
				auto v = new CXonTextValue();
				v->Text = Text;
				return v;
			}
	};

	class CXonBinaryValue : public CXonSimpleValue
	{
		public:
			CBuffer Data;

			void Set(CXonValue * v) override
			{
				Data = dynamic_cast<CXonBinaryValue *>(v)->Data;
			}

			void Set(const ISerializable & v) override
			{
				CMemoryStream s;
				((ISerializable &)v).Write(&s);
				Data = s.Read();
			}
			
			void Get(ISerializable & v) override
			{
				CMemoryStream s;
				s.Write(Data);
				v.Read(&s);
			}
	
			bool Equals(const CXonValue & v) const override
			{
				return Data == ((CXonBinaryValue &)v).Data;
			}
			
			CXonValue * Clone() override
			{
				auto v = new CXonBinaryValue();
				v->Data = Data;
				return v;
			}
	};
}
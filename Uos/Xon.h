#pragma once
#include "Stream.h"
#include "Bool.h"
#include "Int32.h"
#include "Float.h"
#include "Float3.h"
#include "Int32Array.h"
#include "FloatArray.h"
#include "Float2Array.h"
#include "Float3Array.h"
#include "XonValue.h"

namespace uos
{
	enum class EBonHeader : unsigned char
	{
		Null		= 0b00000000,
		Parent		= 0b10000000,
		Last		= 0b01000000,
		HasType		= 0b00100000,
		HasValue	= 0b00010000,
		BigValue	= 0b00001000
	};

	enum class EXonToken
	{
		ChildrenBegin, ChildrenEnd,  NodeBegin, NodeEnd, NameBegin, NameEnd, ValueBegin, ValueEnd, SimpleValueBegin, SimpleValueEnd, AttrValueBegin, AttrValueEnd, End
	};

	class CXon;

	struct IXonWriter
	{
		virtual void								Write(CXon * root) = 0;

		~IXonWriter(){}
	};

	struct IXonReader
	{
		virtual EXonToken							Read()=0;
		virtual EXonToken							ReadNext()=0;
		virtual void								ReadValue(CXonSimpleValue * v)=0;
		virtual void								ReadName(CString & name, CString & type)=0;
		EXonToken				Current;

		~IXonReader(){}
	};

	class UOS_LINKING CXon : public CXonValue
	{
		public:
			CString										Name;
			CString										Id;
			CList<CXon *>								Children;
			CXon *										Parent = null;
			CList<CXon *>								Templates;
			bool										IsTemplatesOwner = false;
			CXonValue *									Value = null;
			CList<CXon *>								Removed;
			bool										IsRemoved = false;
			bool										IsDifferenceDeleted = false;

			const static std::wstring					TypeName;

			UOS_RTTI
			CXon(){}
			CXon(const CXon & xon);
			CXon(CXon && xon);
			CXon(const CString & name);
			virtual ~CXon();

			void										Clear();

			CXon *										GetRoot();
			CXon *										One(const CString & name);
			CArray<CXon *>								Many(const CString & name);
			CArray<CXon *>								ManyOf(const CString & name);
			CXon *										GetOrAdd(const CString & name);

			bool										IsAncestor(CXon * ancestor);

			virtual CXon *								CloneInternal(CXon * parent);

			void										Add(CXon * p);
			CXon *										Add(const CString & name);
			void										Remove(CXon * p);

			void										GiveValue();
			void										Set(const ISerializable & v);
			void										Set(bool v				);
			void										Set(const wchar_t * v	);
			void										Set(const CString & 	);
			void										Set(int v				);
			void										Set(float v				);
			void										Set(CArray<int> & v		);
			void										Set(CArray<float> & v	);
			void										Set(CArray<CFloat2> & v	);
			void										Set(CArray<CFloat3> & v	);

			bool										AsBool();
			int											AsInt32();
			float										AsFloat32();
			CString 									AsString();
			
			bool										Equals(const CXon & a);

			virtual void								Set(CXonValue * v) override;
			virtual bool								Equals(const CXonValue &) const override;;
			virtual CXonValue *							Clone() override;;
		
			template<class T> T Get()
			{
				T v;
				if(Value)
				{
					Value->As<CXonSimpleValue>()->Get(v);
				}
				else
				{
					throw CException(HERE, L"No value");
				}
				return v; 
			}

			template<class T> T Get(const CString & name)
			{
				return One(name)->Get<T>();
			}

			template<class T> T GetOr(const T & t)
			{
				T v;
				if(Value)
				{
					Value->As<CXonSimpleValue>()->Get(v);
					return v; 
				}
				else
				{
					return t;
				}
			}

			template<class T> T GetOr(const CString & name, const T & t)
			{
				return One(name)->GetOr<T>(t);
			}


			template<class T> CXon * Find(const CString & name, T const & v)
			{
				for(auto i : Children)
				{
					if(i->Name == name && i->Get<T>() == v)
					{
						return i;
					}
				}
				return null;
			}


			template<class T> T AsEnum()
			{
				for(int i=0; i<INT_MAX; i++)
				{
					if(ToString((T)i) == Get<CString>())
					{
						return (T)i;
					}
				}

				throw CException(HERE, L"Incorrect value");
			}

		protected:
			virtual void								OnChildValueAssigned();
	};
}
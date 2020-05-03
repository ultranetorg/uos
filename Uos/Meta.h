#pragma once
#include "IType.h"
#include "StaticArray.h"
#include "Shared.h"
#include "String.h"

namespace uos
{
	struct UOS_LINKING CMetaItem
	{
		CString		Key;
		void *		Pointer = null;
		CShared *	Shared = null;
		CBuffer		Buffer;
		CString		String;

		CMetaItem(){}
		~CMetaItem();
		CMetaItem(const CString & key, CShared * value);
		CMetaItem(const CString & key, const void * value);
		CMetaItem(const CString & key, CBuffer & value);
		CMetaItem(const CString & key, const CString & value);
	};
	
/*
	class UOS_FRAMEWORK_CLASS CMeta
	{
		public:
			void										Add(const CString & key, void * value);
			void										Remove(const CString & key);
			void *										Get(const CString & key);
			bool										Contain(const CString & key);


			template<class T> T * GetPointer()
			{
				for(auto i : Metas)
				{
					if(i->Key == T::GetClassName())
					{
						return static_cast<T *>(i->Value);
					}
				}
				throw CException(HERE, L"Meta not found: ", T::GetClassName());
			}
			
			CMeta();
			~CMeta();
		
			CArray<CMetaItem>							Metas;
	};*/
}


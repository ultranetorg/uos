#pragma once
#include "String.h"

namespace uos
{
	class UOS_LINKING CConverter
	{
		public:
			static CString 								ToString(size_t i);
			static CString								ToString(GUID guid);
			static DWORD								ToARGB(const CString & name);
			static GUID									ToGUID(const CString & t);

			template<class T> static CArray<T> ParseArray(const CString & str)
			{
				CArray<T> o;
				o.reserve(str.size());

				std::wistringstream s(str);
				T f;
				while(s >> f || !s.eof())
				{
					if(s.fail())
					{
						s.clear();
						std::wstring dummy;
						s >> dummy;
						if(dummy == L"nan")
						{
							o.push_back(std::numeric_limits<T>::infinity());
						}
						continue;
					}
					else
						o.push_back(f);

				}

				return o;
			}


			template<class T, unsigned int (T::*pMemFn)(void *)> static unsigned int __stdcall MemberToThread(void * pv)
			{
				return (static_cast<T*>(pv)->*pMemFn)(NULL);
			}
	};
}
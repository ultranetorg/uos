#pragma once
#include "Exception.h"

namespace uos
{

	template<class T, int N> class CStaticArray
	{
		public:
			CStaticArray()
			{
				Count = 0;
			}
			
			T & operator [] (int i)
			{
				return Items[i];
			}

			void Add(T & item)
			{
				if(Count < N)
				{
					Items[Count] = item;
					Count++;
					return;
				}
				throw CException(HERE, L"Size exceeded");
			}

			void Remove(T & item)
			{
				for(int i=0; i<Count; i++)
				{
					if(item == Items[i])
					{
						if(i<Count-1)
						{
							Items[i] = Items[Count-1];
						}
						Count--;
						return
					}
				}
				throw CException(HERE, L"Item not found");
			}

			void Remove(int i)
			{
				if(i<Count-1)
				{
					Items[i] = Items[Count-1];
				}
				Count--;
			}
			
			int GetCount()
			{
				return Count;
			}
			
			void Clear()
			{
				Count = 0;
				ZeroMemory(Items, sizeof(T) * N);
			}
		
		private:
			T		Items[N];
			int		Count;
	};
}
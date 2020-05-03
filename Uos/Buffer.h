#pragma once

namespace uos
{
	class UOS_LINKING CBuffer
	{
		public:
			void *										GetData();
			int64_t										GetSize() const;
			void										Set(const void * data, int64_t size);
			void										Add(const void * data, int64_t size);
			void										Add(CBuffer & b);


			CBuffer & operator = (const CBuffer &);
			bool operator != (const CBuffer &);

			bool operator == (const CBuffer & b) const
			{
				if(GetSize() != b.GetSize())
				{
					return false;
				}

				return memcmp(((CBuffer *)this)->GetData(), ((CBuffer *)&b)->GetData(), (size_t)GetSize()) == 0;
			}

			CBuffer();
			CBuffer(CBuffer && b);
			CBuffer(const CBuffer & b);
			CBuffer(int64_t size);
			CBuffer(const void * data, int64_t size);
			~CBuffer();
			
		private:
			uint64_t 									Data;
			int64_t										Size = 0;
	};

}

#pragma once
#include "Converter.h"

namespace uos
{
	enum class ETextEncoding
	{
		UTF8
	};

	class CTextReader
	{
		public:
			CString		Text;

			CTextReader(CStream * s, ETextEncoding e)
			{
				CBuffer b = s->Read();

				if(e == ETextEncoding::UTF8)
				{
					if(b.GetSize() >= 3 &&	((unsigned char *)b.GetData())[0] == 0xEF && 
											((unsigned char *)b.GetData())[1] == 0xBB && 
											((unsigned char *)b.GetData())[2] == 0xBF) 
					{
						assert(b.GetSize() < INT_MAX);
						
						Text = CString::FromUtf8((char *)b.GetData()+3, (int)b.GetSize()-3);
					}
				}
			}
			~CTextReader()
			{

			}
	};
}

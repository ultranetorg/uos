#pragma once
#include "Buffer.h"

namespace uos
{
	class UOS_LINKING CBase64
	{
		public:
			static std::wstring							Encode(unsigned char const* bytes_to_encode, int64_t in_len);
			static CBuffer								Decode(std::wstring const& encoded_string);
	};
}


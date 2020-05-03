#pragma once

namespace uos
{
	class UOS_LINKING CBase58
	{
		public:
			/**
			 * Encode a byte sequence as a base58-encoded string.
			 * pbegin and pend cannot be nullptr, unless both are.
			 */
			static std::wstring Encode(const unsigned char* pbegin, const unsigned char* pend);
	
			/**
			 * Encode a byte vector as a base58-encoded string
			 */
			static std::wstring Encode(const std::vector<unsigned char>& vch);
	
			/**
			 * Decode a base58-encoded string (psz) into a byte vector (vchRet).
			 * return true if decoding is successful.
			 * psz cannot be nullptr.
			 */
			static bool Decode(const wchar_t * psz, std::vector<unsigned char>& vchRet);
	
			/**
			 * Decode a base58-encoded string (str) into a byte vector (vchRet).
			 * return true if decoding is successful.
			 */
			static bool Decode(const std::wstring& str, std::vector<unsigned char>& vchRet);
	
			///**
			// * Encode a byte vector into a base58-encoded string, including checksum
			// */
			//std::string EncodeBase58Check(const std::vector<unsigned char>& vchIn);
			//
			///**
			// * Decode a base58-encoded string (psz) that includes a checksum into a byte
			// * vector (vchRet), return true if decoding is successful
			// */
			//bool DecodeBase58Check(const char* psz, std::vector<unsigned char>& vchRet);
			//
			///**
			// * Decode a base58-encoded string (str) that includes a checksum into a byte
			// * vector (vchRet), return true if decoding is successful
			// */
			//bool DecodeBase58Check(const std::string& str, std::vector<unsigned char>& vchRet);
	};
}



#pragma once

namespace uos
{
	class UOS_LINKING CCodePosition 
	{
		public:
			std::wstring		ClassMethod;
			int					Line;

			CCodePosition();
			CCodePosition(const wchar_t * method, int line);
	};

	class UOS_LINKING CException
	{
		public:
			CCodePosition		Source;
			std::wstring		Message;

			CException(wchar_t const * m, int l, std::wstring const & msg);
			~CException();

		protected:
			CException();
	};

	class UOS_LINKING CLastErrorException : public CException
	{
		public:
			CLastErrorException(const wchar_t * m, int l, int err, const wchar_t * f, ...);
			~CLastErrorException(){}
	};
	
	class UOS_LINKING CAttentionException : public CException
	{
		public:
			CAttentionException(const wchar_t * m, int l, const wchar_t * f, ...);
			~CAttentionException(){}
	};
	
}
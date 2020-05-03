#pragma once
#include "ISerializable.h"
#include "Array.h"
#include "List.h"

namespace uos
{
	typedef std::string CAnsiString;

	class CString;

	class IEntity
	{
		public:
			virtual ~IEntity(){}
	};

	class IComparable
	{
		public:
			virtual int Compare(IComparable * a)=0;
	};

	class IStringEntity : public IEntity
	{
		public:
			virtual void	SetValue(CString const & v)=0;
			virtual CString GetValue()=0;
	};
	
	class CString : public std::wstring, public ISerializable, public IStringEntity, public IComparable
	{
		public:
			UOS_LINKING static const std::wstring	TypeName;

			UOS_LINKING CString();
			UOS_LINKING CString(const wchar_t * p, int n);
			UOS_LINKING CString(const wchar_t * v);
			UOS_LINKING CString(const wchar_t * b, const wchar_t * e);
			UOS_LINKING CString(const std::wstring & v);
			UOS_LINKING CString(const_iterator b, const_iterator e);
			UOS_LINKING CString(size_t n, wchar_t c);
		

			UOS_LINKING static int					Compare(wchar_t const * a, wchar_t const * b);
			
			UOS_LINKING static CString				FromAnsi(const CAnsiString & source);
			UOS_LINKING static CString				FromAnsi(const char * source);
			UOS_LINKING static CString				FromUtf8(const char * source, int n);
			UOS_LINKING static CString				FromUtf8(CAnsiString const & a);
			UOS_LINKING CArray<char>				ToUtf8() const;
			UOS_LINKING CAnsiString					ToAnsi() const;

			UOS_LINKING CString & operator=			(const CString & s);
			UOS_LINKING CString & operator=			(ISerializable * s);
			
			UOS_LINKING CString						Substring(size_t offset = 0, size_t count = INT_MAX) const;
			UOS_LINKING CString						Substring(wchar_t const separ, int index) const;
			UOS_LINKING CString						Substring(CString const & separ, int index) const;
			
			UOS_LINKING std::wstring				GetTypeName();
			UOS_LINKING bool						Equals(const ISerializable & a) const;
			UOS_LINKING ISerializable *				Clone();
			UOS_LINKING void						Read(const std::wstring & v);
			UOS_LINKING void						Read(CStream * s);
			UOS_LINKING void						Write(std::wstring & s);
			UOS_LINKING int64_t						Write(CStream * s);
			
			UOS_LINKING CArray<CString>				ToLines() const;
			UOS_LINKING CString						ToUpper();
			UOS_LINKING CString						ToLower();
			UOS_LINKING bool						EqualsInsensitive(const CString & v) const;
			UOS_LINKING bool						StartsWith(const CString & v) const;
			UOS_LINKING bool						EndsWith(const CString & v) const;
			UOS_LINKING void						SelfReplace(const wchar_t * find_str, const wchar_t * replace_str);
			UOS_LINKING CString						ReplaceLast(const CString &find_str, const CString &replace_str) const;
			UOS_LINKING CString						Replace(const CString &find_str, const CString &replace_str) const;
			UOS_LINKING CArray<CString>				Split(const wchar_t * s, bool compress = false) const;;
			UOS_LINKING CList<CString>				SplitToList(const wchar_t * s, bool compress = false) const;
			UOS_LINKING int							Count();

			UOS_LINKING virtual void				SetValue(CString const & v) override;
			UOS_LINKING virtual CString				GetValue() override;
			UOS_LINKING virtual int					Compare(IComparable * v) override;

			template<class Pred> bool Has(const Pred p)
			{
				return std::find_if(begin(), end(), p) != end();
			}

			template<class A, class P> static CString Join(A & a, P gettext, const wchar_t * sep)
			{
				CString o;
				for(auto & i : a)
				{
					auto & t = gettext(i);
					if(!o.empty())
					{
						o += sep;
					}
					o += t;
				}
				return o;
			}

			template<class A> static CString Join(A & a, const wchar_t * sep)
			{
				CString o;
				for(auto & i : a)
				{
					if(!o.empty())
					{
						o += sep;
					}
					o += i;
				}
				return o;
			}


			template <typename T> static T ProcessArg(T value) noexcept
			{
				return value;
			}

			template <typename T> static T const * ProcessArg(std::basic_string<T> const & value) noexcept
			{
				return value.c_str();
			}

			static wchar_t const * ProcessArg(CString const & value) noexcept
			{
				return value.c_str();
			}

			#pragma warning(push)
			#pragma warning(disable: 4996)

			template<typename ... Args> static CString Format(const std::wstring& format, Args const & ... args)
			{
				const auto fmt = format.c_str();
				const size_t size = _snwprintf(nullptr, 0, fmt, ProcessArg(args) ...) + 1;
				auto buf =  (wchar_t *)_alloca(size * sizeof(wchar_t));
				_snwprintf(buf, size, fmt, ProcessArg(args) ...);
				return CString(buf, buf + size - 1);
			}

			template<typename ... Args> void Printf(const std::wstring& format, Args const & ... args)
			{
				const auto fmt = format.c_str();
				const size_t size = _snwprintf(nullptr, 0, fmt, ProcessArg(args) ...) + 1;
				auto buf =  (wchar_t *)_alloca(size * sizeof(wchar_t));
				_snwprintf(buf, size, fmt, ProcessArg(args) ...);
				assign(buf, buf + size - 1);
			}

			#pragma warning(pop)


	};
}
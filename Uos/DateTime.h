#pragma once
#include "String.h"

namespace uos
{
	class UOS_LINKING CDateTime : public ISerializable, public IStringEntity, public IComparable
	{
		public:
			time_t Value;

			const static CDateTime	Min;
			const static CString	DefaultFormat;

			CDateTime();
			CDateTime(time_t t);
			CDateTime(int year, int mon, int day, int hour, int minute, int second, int zone);


			static CDateTime							Now();
			static CDateTime							UtcNow();
			static CDateTime							Parse(CString const & t, CString const & f);

			CString										ToString(CString const & f);
			CString										ToString();

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;

			virtual void								SetValue(CString const & v) override;
			virtual CString								GetValue() override;
			virtual int									Compare(IComparable * v) override;

			double operator - (const CDateTime & a);

			bool operator < (const CDateTime & a)
			{
				return  (*this - a) < 0;
			}

			bool operator > (const CDateTime & a)
			{
				return  (*this - a) > 0;
			}

			bool operator == (const CDateTime & a)
			{
				return  (Value - a.Value) == 0;
			}

			bool operator != (const CDateTime & a)
			{
				return  (Value - a.Value) != 0;
			}

	};
}



#pragma once
#include "String.h"
#include "Float.h"

namespace uos
{
	enum EAreaField
	{
		EAreaField_Width = 1,
		EAreaField_Height = 2,
		EAreaField_Depth = 4,
		EAreaField_All = 0xffffffff
	};

	class UOS_LINKING CSize : public ISerializable
	{
		public:
			float W;
			float H;
			float D;

			const static std::wstring					TypeName;
			static CSize								Nan;
			static CSize								Empty;
			static CSize								Max;
			
			CSize();
			CSize(float w, float h, float d);

			std::wstring								GetTypeName();
			void										Read(CStream * s);
			int64_t										Write(CStream * s);
			void										Write(std::wstring & s);
			void										Read(const std::wstring & b);
			ISerializable *								Clone();
			bool										Equals(const ISerializable & a) const;

			bool										IsReal() const;
			bool										IsEmpty() const;
			CString										ToNiceString();

			bool										operator == (const CSize & a) const;
			bool										operator != (const CSize & a) const;

			CSize										operator * (float a);
			operator									bool() const;

	};
}

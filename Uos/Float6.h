#pragma once
#include "String.h"

namespace uos
{
	class UOS_LINKING CFloat6 : public ISerializable
	{
		public:
			float	LF;
			float	RT;
			float	BM;
			float	TP;
			float	FT;
			float	BK;

			CFloat6();
			CFloat6(CString const & v) { Read(v); }
			explicit CFloat6(float a);
			CFloat6(float rt, float lf, float bm, float tp, float ft, float bk);

			//CFloat6 &									operator = (float a);

			const static std::wstring					TypeName;
			static const CFloat6 						Nan;

			CString										ToString();

			std::wstring								GetTypeName() override;
			void										Read(CStream * s) override;
			int64_t										Write(CStream * s) override;
			void										Write(std::wstring & s) override;
			void										Read(const std::wstring & b) override;
			ISerializable *								Clone() override;
			bool										Equals(const ISerializable & a) const override;
	};
}
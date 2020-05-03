#pragma once

namespace uos
{
	enum class ECardTitleMode
	{
		Null, No, Right, Left, Top, Bottom
	};

	UOS_WORLD_LINKING wchar_t *				ToString(ECardTitleMode a);
	UOS_WORLD_LINKING ECardTitleMode		ToAvatarTitleMode(const CString & name);

	struct CAvatarMetrics
	{
		CSize					FaceSize = CSize::Nan;
		CSize					TextSize = CSize::Nan;
		CFloat6					FaceMargin = CFloat6(0);

		CAvatarMetrics()
		{
		}
/*

		CAvatarMetrics(float w, float h, float m)
		{
			FaceSize	= CSize(w, h, 0);
			HTextSize	= CSize(w * 3, h, 0);
			VTextSize	= CSize(w, h * 2, 0);
			FaceMargin	= CFloat6(m);
		}
*/

		CAvatarMetrics(CXon * p)
		{
			FaceSize	= p->Get<CSize>(L"FaceSize");
			TextSize	= p->Get<CSize>(L"TextSize");
			FaceMargin	= p->Get<CFloat6>(L"Margin");
		}

		void Save(CXon * p)
		{
			p->Add(L"FaceSize")->Set(FaceSize);
			p->Add(L"TextSize")->Set(TextSize);
			p->Add(L"Margin")->Set(FaceMargin);
		}
	};
}

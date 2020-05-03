#pragma once
#include "EngineLevel.h"

namespace uos
{
	enum class EScreenType
	{
		Null = 0, Display = 1, Window = 2
	};

	static wchar_t * ToString(EScreenType e)
	{
		static wchar_t * names[] = {L"Null", L"Display", L"Window"};
		return names[(int)e];
	}


	class CScreen : public CEngineEntity
	{
		public:
			CGdiRect									Rect;
			CGdiRect									NRectWork;

			CString										Name;
			CList<CString>								Tags;

			virtual void								Show(bool) = 0;
			virtual void								SetCursor(HCURSOR Default) = 0;

			CFloat2 NativeToScreen(CFloat2 & p)
			{
				auto h = Rect.Height;
				return CFloat2(p.x, float(h) - p.y - 1);
			}

			UOS_RTTI
			CScreen(CEngineLevel * l) : CEngineEntity(l){}
			virtual ~CScreen(){}
	};
}

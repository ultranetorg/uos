#pragma once
#include "Element.h"

namespace uos
{
	class UOS_WORLD_LINKING CText : public CElement
	{
		public:
			CStyle *									Style;
			CEngine *									Engine;
			CString										Text;
			CFloat4										Color = CFloat4(NAN);
			bool										Wrap = false;
			bool										Ellipsis = true;
			CFont *										Font = null;
			EXAlign										XAlign = EXAlign::Left;
			EYAlign										YAlign = EYAlign::Top;
			int											TabLength = 4;
			bool										Shadow = false;
			IStringEntity *								Entity = null;

			UOS_RTTI
			CText(CWorldLevel * l, CStyle * s, CString const & name = GetClassName(), bool active = false);
			~CText();

			void										SetFont(CFont * name);
			void										SetText(const CString & t);
			void										SetWrap(bool e);
			void										SetColor(CFloat4 & c);
			int											GetPosition(const CString & s, float x);

			virtual void								Draw();
			CSize										Measure(float wmax, float hmax);

			void virtual								LoadProperties(CStyle * s, CXon * n);

			virtual void								SetEntity(IEntity * e) override;
			virtual CElement *							Clone() override;

			void										UpdateLayout(CLimits const & l, bool apply);

	};
}

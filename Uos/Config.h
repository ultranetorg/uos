#pragma once
#include "Level2.h"

namespace uos
{
	class UOS_LINKING CConfig : public IType
	{
		public:
			CLevel2 *									Level;
			CString										DefaultUri;
			CString										CustomUri;

			CXonDocument *								Root = null;
			CXonDocument *								DefaultDoc = null;

			void										Save();

			CXon *										GetRoot();

			UOS_RTTI
			CConfig(CLevel2 * l, CString & durl, CString & curl);
			~CConfig();

	};
}
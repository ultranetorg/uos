#pragma once
#include "Font.h"
#include "DirectSystem.h"

namespace uos
{
	class UOS_ENGINE_LINKING CFontFactory
	{
		public:
			CFont *										GetFont(const CString & family, float size, bool bold, bool italic);
			CFont *										GetFont(CFontDefinition & desc);

			CFontFactory(CEngineLevel * l, CDirectSystem * ge);
			~CFontFactory();
		
		private:
			CList<CFont *>								Fonts;
			CEngineLevel *								Level;
			CDirectSystem *								GraphicEngine;
	};
}
#pragma once
#include "Style.h"

namespace uos
{
	struct CWorldLevel : public CLevel2
	{
		CServer *										Server;
		CEngine *										Engine;
		CMaterialPool *									Materials;
		CStyle *										Style;
		CStorage *										Storage;

		CWorldLevel(CLevel2 * l) : CLevel2(*l){}
	};
}
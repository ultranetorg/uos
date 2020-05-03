#pragma once
#include "Event.h"

namespace uos
{
	class ILevel
	{
		public:
			CEvent<int, ILevel *>						LevelCreated;
			CEvent<int, ILevel *>						LevelDestroyed;

			virtual ~ILevel(){};
	};
}
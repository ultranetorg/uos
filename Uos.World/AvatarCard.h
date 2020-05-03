#pragma once
#include "Card.h"

namespace uos
{
	class UOS_WORLD_LINKING CAvatarCard : public CCard
	{
		public:
			CObject<CAvatar>							Avatar;
			CObject<CWorldEntity>						Entity;

			UOS_RTTI
			CAvatarCard(CWorld * l, const CString & name = GetClassName());
			~CAvatarCard();

			void										SetAvatar(CUol & e, CString const & dir);
			void										SetEntity(CUol & a);

			void										OnDependencyDestroying(CNexusObject * o);
			void										OnTitleChanged(CWorldEntity *);

			void										SetMetrics(CAvatarMetrics & m);

			void										Save(CXon * x);
			void										Load(CXon * x);
	};
}
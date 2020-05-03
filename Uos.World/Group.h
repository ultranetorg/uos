#pragma once
#include "Avatar.h"
#include "Entity.h"

namespace uos
{
	class CGroup : public CWorldEntity
	{
		public:
			CWorldLevel	*								Level;
			CList<CObject<CWorldEntity>>				Entities;

			UOS_RTTI
			CGroup(CWorldLevel * l, CString const & name = CGuid::Generate64(GetClassName())) : CWorldEntity(l->Server, name)
			{
				Level = l;
				SetDirectories(MapRelative(L""));
				SetDefaultInteractiveMaster(AREA_MAIN);
			}

			~CGroup()
			{
				Save();
			}

			void SaveInstance() override
			{
				CTonDocument d;

				d.Add(L"Title")->Set(Title);

				for(auto & i : Entities)
				{
					d.Add(L"Entity")->Set(i.Url);
				}
				
				SaveGlobal(d, GetClassName() + L".xon");
			}

			void LoadInstance() override
			{
				CTonDocument d;
				LoadGlobal(d, GetClassName() + L".xon");

				SetTitle(d.Get<CString>(L"Title"));

				for(auto i : d.Many(L"Entity"))
				{
					Entities.push_back(i->Get<CUol>());
				}
			}
	};
}
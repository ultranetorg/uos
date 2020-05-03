#pragma once
//#include "AvatarMetrics.h"
//#include "WorldNode.h"

#include "Avatar.h"

namespace uos
{
	class CDragItem
	{
		public:
			CString			Class;
			CUrl			Object;
			CBuffer			Binary;
			CUol			Owner;

			CDragItem(CUol & owner, CUrl & o)
			{
				Owner = owner;
				Object = o;
			}

			virtual ~CDragItem()
			{
			}
	};

	class IDropTarget
	{
		public:
			virtual CModel *						Enter(CArray<CDragItem> & d, CAvatar * n)=0;
			virtual bool								Test(CArray<CDragItem> & d, CAvatar * n)=0;
			virtual void								Leave(CArray<CDragItem> & d, CAvatar * n)=0;
			virtual void								Drop(CArray<CDragItem> & d, CPick & a)=0;
			
			virtual ~IDropTarget(){}
	};
}

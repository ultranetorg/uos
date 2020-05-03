#pragma once

namespace uos
{
	class IFieldOperations
	{
		public:
			//virtual CWorldNode * 						AddItem(CAvatar * n, const CFloat3 & p)=0;
			virtual void								MoveAvatar(CAvatar * a, CTransformation & p)=0;
			virtual void								DeleteAvatar(CAvatar * a)=0;
			virtual CPositioning *						GetPositioning(CPick & p)=0;
			virtual CAvatarMetrics						GetMetrics(CString const & am, ECardTitleMode tm, const CSize & a)=0;
			virtual CMaterial *							GetOfflineMaterial()=0;
						
			virtual void								AddIconMenu(IMenuSection * ms, CAvatar * a)=0;
			virtual void								AddTitleMenu(IMenuSection * ms, CAvatar * a)=0;

			virtual ~IFieldOperations(){}
	};

}
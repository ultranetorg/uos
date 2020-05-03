#pragma once
#include "Avatar.h"
#include "SpaceCollection.h"

namespace uos
{
	class CArea;
	class CWorldServer;

	enum class EAddLocation
	{
		Null, Known, Front, Back
	};

	struct CPlacement
	{
		CString											Name;
		CString											Class;
		///CString										Layout;
		CArea *											Area			= null;
		CTransformation									Transformation	= CTransformation::Nan;
		CSize											Size			= CSize::Nan;
		bool											SizeDetermined	= false;
		//bool											TransformationDetermined = false;
		ELifespan										Lifespan = ELifespan::Null;
	};

	class UOS_WORLD_LINKING CArea : public virtual IType, public virtual CShared
	{
		public:
			CString										Name;
			CWorldServer *								Level;
			bool										Interactive = false;
			ELifespan									Lifespan = ELifespan::Null;
			CArea *										Parent = null;
			CView *										View = null;
			CList<CPlacement *>							Areas;
			CSpaceCollection<CVisualSpace>				VisualSpaces;
			CSpaceCollection<CActiveSpace>				ActiveSpaces;
			CTransformation								Transformation = CTransformation::Identity;
			std::function<CSize(CViewport *)>			MaxSize;
			//CString										DefaultInteractiveMaster;
			CList<CString>								Tags;
			CArea *										TransformationParent = null;				
			CString										Directory;

			UOS_RTTI
			CArea(CWorldServer * l, CString const & name = CGuid::Generate64(GetClassName()));
			~CArea();

			virtual bool								Add(CArea * a, EAddLocation l);
			virtual void								Remove(CArea * a);
			void										Forget(CString const & name);
			void virtual								Open(CArea * a, EAddLocation l, CViewport * vp, CPick & pick, CTransformation & origin);
			void virtual								Close(CArea * a);
			void virtual								Activate(CArea * a, CViewport * vp, CPick & pick, CTransformation & origin);
			void										Remember(CArea * a);

			virtual CSpaceBinding<CVisualSpace> &		AllocateVisualSpace(CViewport * vp);
			virtual CSpaceBinding<CActiveSpace> &		AllocateActiveSpace(CViewport * vp);
			void virtual								DeallocateVisualSpace(CVisualSpace * s);
			void virtual								DeallocateActiveSpace(CActiveSpace * s);

			virtual void								Save(CXon * x);
			virtual void								Load(CXon * p);

			virtual void								Save();
			virtual void								Load();

			bool										ContainsDescedant(CArea * a);
			bool										IsUnder(CArea * parent);
			bool										UnderInteractive();
			CArea *										Find(CString const & name);
			CArea *										Match(CString const & tag);
			CPlacement * FindPlacement(CArea * a);

			CSize										GetActualMaxSize(CViewport * vp);
			CView *										GetActualView();
			void										SetView(CView * v);

			virtual void								Transform(const CTransformation & t);
			
			CSize virtual								Measure();

			template<class T> T * AncestorOf()
			{
				auto p = this;
				while(p && dynamic_cast<T *>(p) == null)
				{
					p = p->Parent;
				}
				return dynamic_cast<T *>(p);
			}

	};
}
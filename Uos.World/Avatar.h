#pragma once
#include "AvatarMetrics.h"
#include "Element.h"
#include "Entity.h"
#include "Positioning.h"

namespace uos
{
	class IAvatarProtocol;
	class CUnit;
	class CWorld;

	enum class ELifespan
	{
		Null, Visibility, Session, Permanent
	};

	enum class EModelAction
	{
		Null, Positioning, Transfering
	};
	
	enum class EPreferedPlacement
	{
		Null, Default, Exact, Convenient
	};

	struct CShowParameters : public CExecutionParameters
	{
		CPick			Pick;
		CActiveArgs * 	Args = null;
		CAnimation 		Animation;
		bool			PlaceOnBoard = false;
		bool			Activate = true;
		bool			Maximize = false;

		CShowParameters(){}
		CShowParameters(CPick & p) : Pick(p) {}
		CShowParameters(CInputArgs * arg, CStyle * s)
		{
			if(auto a = arg->As<CMouseArgs>())
			{
				Pick = a->Pick;
				PlaceOnBoard = a->Class == EInputClass::Mouse && a->Control == EControl::MiddleButton;
			}
			if(auto a = arg->As<CTouchArgs>())
			{
				Pick = a->GetPick();
			}

			Animation = s->GetAnimation(L"Animation");
			Args = arg;
			Args->Take();
		}

		~CShowParameters()
		{
			if(Args)
				Args->Free();
		}
	};

	struct CHideParameters : public CExecutionParameters
	{
		CActiveArgs * 		Args = null;
		CAnimation 			Animation;
		CTransformation		End = CFloat3::Nan;

		CHideParameters(){}
		CHideParameters(CActiveArgs * a, CStyle * s)
		{
			Args = a;
			Args->Take();
			Animation = s->GetAnimation(L"Animation");
		}

		~CHideParameters()
		{
			if(Args)
				Args->Free();
		}
	};
	
	class UOS_WORLD_LINKING CAvatar : public CElement, public CNexusObject 
	{
		public:
			CProtocolConnection<IAvatarProtocol>		Protocol;
			CWorld *									World;

			UOS_RTTI
			CAvatar(CWorld * l, CServer * s, CString const & name); 

			using CNexusObject::Load;

			virtual void								SetEntity(CUol & e);
			virtual void								DetermineSize(CSize & smax, CSize & s);
	};
	
	class UOS_WORLD_LINKING CModel : public CAvatar
	{
		public:
			ELifespan									Lifespan;
			CUnit *										Unit = null;
			bool										UseHeader = false;
			CMap<CString, EPreferedPlacement>			PreferedPlacement;
			CList<CString>								Tags = {L"Apps"};

			UOS_RTTI
			CModel(CWorld * l, CServer * s, ELifespan life, CString const & name);
			virtual ~CModel();

			virtual void								Open(CUnit * a);
			virtual	void								Close(CUnit * a);

			virtual CTransformation						DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t);

			EPreferedPlacement GetPreferedPlacement();
	};
	
	class UOS_WORLD_LINKING CStaticAvatar : public CAvatar
	{
		public: 
			UOS_RTTI
			CStaticAvatar(CWorld * l, CServer * s, CXon * r, IMeshStore * mhs, IMaterialStore * mts, const CString & name = CGuid::Generate64(GetClassName()));
			CStaticAvatar(CWorld * l, CServer * s, CElement * e, const CString & name);
			virtual ~CStaticAvatar();
	};

	class IAvatarProtocol : public virtual IProtocol
	{
		public:
			virtual CNexusObject *						GetEntity(CUol & a)=0;
			virtual CAvatar *							CreateAvatar(CUol & a)=0;
			virtual CList<CUol>							GenerateSupportedAvatars(CUol & o, CString const & type)=0;
			virtual void								DestroyAvatar(CAvatar * a) = 0;

			virtual ~IAvatarProtocol(){}
	};

	class IFaceTitleAvatar
	{
		public:
			virtual void								SetAppearance(ECardTitleMode tm, CAvatarMetrics & m)=0;
	
			virtual ~IFaceTitleAvatar(){}
	};
}

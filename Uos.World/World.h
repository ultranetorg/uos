#pragma once
#include "IDropTarget.h"
#include "Unit.h"
#include "WorldView.h"
#include "PositioningArea.h"

namespace uos
{
	enum class EWorldAction
	{
		Null, LeftSlide, RightSlide, TopSlide, BottomSlide
	};

	struct CWorldMode
	{
		CString			Name;
		CTonDocument *	WorldConfig;
		CTonDocument *	EngineConfig;
	};

	class CWorld : public CWorldLevel, public virtual IProtocol
	{
		public:
			CString													Name;
			CString													Complexity;
			bool													Free3D;
			bool													FullScreen;
			bool													Tight;

			CList<CWorldMode>										Modes;
						
			CList<CScreenViewport *>								Viewports;
			CScreenViewport *										MainViewport;

			CTreeVisualGraph *										ThemeVisualGraph;
			CTreeVisualGraph *										MainVisualGraph;
			CActiveGraph *											MainActiveGraph;

			CWorldView *											ThemeView = null;
			CWorldView *											MainView = null;
			CWorldView *											HudView = null;
			CWorldView *											NearView = null;

			CString													Layout;

			CEvent<CUnit *, CTransformation &, CShowParameters *>	UnitOpened;	
			CEvent<CUnit *>											UnitClosed;	
			CEvent<EWorldAction>									ActionActivated;
			CEvent<CInputArgs *>									ServiceSurfaceButtonEvent;

			bool													Initializing = false;
			bool													Starting = true;

			CArea *													Area = null;
			CArea *													ServiceBackArea = null;
			CArea *													ThemeArea = null;
			CPositioningArea *										BackArea = null;
			CPositioningArea *										FieldArea = null;
			CPositioningArea *										MainArea = null;
			CPositioningArea *										HudArea = null;
			CPositioningArea *										TopArea = null;
			CArea *													ServiceFrontArea = null;

			CModel *												Sphere = null;

			CMap<EControl, std::function<void(EControl)>>			GlobalHotKeys;

			virtual CGroup *							CreateGroup(CString const & name)=0;

			virtual void								Switch(CString const & mode, CString const & layout)=0;

			virtual CUol								GenerateAvatar(CUol & entity, CString const & type)=0;
			virtual CAvatar *							CreateAvatar(CUol & avatar, CString const & dir)=0;
			virtual void								DestroyAvatar(CAvatar * a)=0;
						
			virtual CUnit *								AllocateUnit(CModel * m)=0;
			virtual CUnit *								AllocateUnit(CUol & entity, CString const & type)=0;
			//virtual CObject<CAvatarization>				RestoreAvatar(CUol & avatar, CString const & type)=0;
			//virtual CAvatarization *					Avatarize(CModel * m)=0;
			virtual void								Dealloc(CUnit * a)=0;
			//virtual CUnit *								FindUnit(CString const & name, CUol & avatar, CUol & entity)=0;
			virtual CUnit *								OpenEntity(CUol & u, CString const & mainSpace, CShowParameters * f)=0;
			//virtual CAllocation *						OpenAllocation(CUol & u, CString const & mainSpace, CShowFeatures & f)=0;
			virtual void								Show(CUnit * a, CString const & mainSpace, CShowParameters * f)=0;
			virtual void								Hide(CUnit * a, CHideParameters * hf)=0;
			
			virtual void								RunAnimation(CElement * n, CAnimated<CTransformation> a)=0;
			
			virtual void								Attach(CElement * m, CUol & l)=0;
			virtual void								Detach(CElement * m, CUol & l)=0;
			virtual bool								IsAttachedTo(CUol & l, CElement * to)=0;
			virtual bool								IsAttachedTo(CUol & l, CUol & to)=0;
			
//			virtual void								AttachSpace(CWorldSpace * s, CVisualGraph * vg, CActiveGraph * ag)=0;
//			virtual void								DetachSpace(CWorldSpace * s, CVisualGraph * vg, CActiveGraph * ag)=0;
			
			virtual void								Drag(CArray<CDragItem> & d)=0;
			virtual void								CancelDragDrop()=0;

			virtual CProtocolConnection<IAvatarProtocol> FindAvatarSystem(CUol & e, CString const & type)=0;

			CWorld(CLevel2 * l) : CWorldLevel(l){}
			virtual ~CWorld(){}
	};
}

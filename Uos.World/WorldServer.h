#pragma once
#include "World.h"
#include "IWorldFriend.h"
#include "IUwmProtocol.h"
#include "CylindricalPositioning.h"
#include "PolygonPositioning.h"
#include "Sphere.h"

namespace uos
{
	class CMobileSkinModel;
	
	class CWorldServer : public CServer, public CWorld, public IExecutorProtocol, public IViewStore, public IUwmProtocol, public IAvatarProtocol
	{
		public:
			CList<CUnit *>								Units;
			CList<CUnit *>								Showings;
			CArray<CUnit *>								Movings;
			CList<CUnit *>								Hidings;
			int											SkipEventId = 0;

			CPositioningCapture							Capture;
			
			CDiagnostic *								Diagnostic;
			CDiagGrid									DiagGrid;
			IPerformanceCounter *						PfcUpdate;
			
			CTonDocument *								WorldConfig;
			CTonDocument *								AreasConfig;
			CConfig *									EngineConfig;
			
			CArray<CDragItem>							Drags;
			CObject<CAvatar>							DragAllocation = null;
			CAvatar *									DragDefaultAvatar = null;
			CAvatar *									DragCurrentAvatar = null;
			CElement *									DropTarget = null;

			bool										InSpecialArea = false;
			bool										InStopping = false;
	
			CList<CScreenRenderTarget *>				Targets;
			CMap<CViewport *, CScreenRenderLayer *>			RenderLayers;
			CMap<CViewport *, CActiveLayer *>			ActiveLayers;
								   
			int											ScreenshotId;

			float										Z;
			float										Fov;

			UOS_RTTI
			CWorldServer(CLevel2 * l, CServerInfo * si);
			~CWorldServer();

			void										Start(EStartMode sm) override;
			IProtocol * 								Connect(CString const & pr) override;
			void										Disconnect(IProtocol * c) override;
			
			CGroup *									CreateGroup(CString const & name) override;

			virtual void								InitializeViewports(){}
			virtual void								InitializeGraphs(){}
			virtual void								InitializeView(){}
			virtual void								InitializeAreas(){}
			virtual void								InitializeModels(){}

			void										Switch(CString const & mode, CString const & layout);

			void										Start();

			virtual void								Execute(const CUrq & o, CExecutionParameters * p) override;
			virtual bool								CanExecute(const CUrq & o) override;

			CUol										GenerateAvatar(CUol & entity, CString const & type) override;
			CAvatar *									CreateAvatar(CUol & avatar, CString const & dir) override;
			void										DestroyAvatar(CAvatar * a) override;

			CUnit *										AllocateUnit(CModel * m);
			CUnit *										AllocateUnit(CUol & entity, CString const & type);
			void										Dealloc(CUnit * a);
			CUnit *										FindUnit(CUol & entity) ;
			
			CUnit *										GetUnit(CActive * a);
			CUnit *										FindGroup(CArea * a);

			CUnit *										OpenEntity(CUol & u, CString const & mainSpace, CShowParameters * f) override;
			void										OpenUnit(CUnit * a, CString const & mainSpace, CShowParameters * f);
			void										Show(CUnit * a, CString const & mainSpace, CShowParameters * f) override;
			void										Hide(CUnit * a, CHideParameters * hf) override;
			
			void virtual								StartShowAnimation(CArea * a, CShowParameters * f, CTransformation & from, CTransformation & to);
			void virtual								StartHideAnimation(CArea * a, CHideParameters * f, CTransformation & from, CTransformation & to, std::function<void()> hide);
			
			virtual CList<CUnit *>						CollectHidings(CArea * a, CArea * master){ return {}; }
			//virtual void								OpenAtSameArea(CAllocation * a, CArea * prevArea, CShowParameters * f){}

			void										RunAnimation(CElement * n, CAnimated<CTransformation> a) override;
			void										RunAnimation(CArea * n, CAnimated<CTransformation> a);

			void										Attach(CElement * m, CUol & l) override;
			void										Detach(CElement * m, CUol & l) override;
			bool										IsAttachedTo(CUol & l, CElement * to) override;
			bool										IsAttachedTo(CUol & l, CUol & to) override;

			//void										AttachSpace(CWorldSpace * s, CVisualGraph * vg, CActiveGraph * ag) override;
			//void										DetachSpace(CWorldSpace * s, CVisualGraph * vg, CActiveGraph * ag) override;

			void										Drag(CArray<CDragItem> & d) override;
			void										CancelDragDrop() override;

			void										OnNexusStopping();
			void										OnDiagnosticsUpdating(CDiagnosticUpdate & a);
			//void										OnMoveInput(CActive * r, CActive * s, CMouseArgs *);
			//void										OnToggleInput(CActive * r, CActive * s, CToggleInputArgs &);
			//void										OnStateChanged(CActive *, CActive *, CActiveStateArgs *);
			
			///void										ShowMenu(CCursorEventArgs & a);
			
			virtual CView *								Get(const CString & name) override;
			
			CProtocolConnection<IAvatarProtocol>		FindAvatarSystem(CUol & e, CString const & type) override;

			virtual CElement *							CreateElement(CString const & name, CString const & type) override;

			CNexusObject *								CreateObject(CString const & name) override;

			virtual CNexusObject *						GetEntity(CUol & a) override;
			virtual CList<CUol>							GenerateSupportedAvatars(CUol & e, CString const & type) override;
			virtual CAvatar *							CreateAvatar(CUol & o) override;
	};

}

#pragma once
#include "ActiveSpace.h"
#include "InputSystem.h"

namespace uos
{
	class CActiveEvent
	{
		public:
			CActiveEvent *			Parent;
			EActiveEvent			Type;
			CActive *				Receiver;
			CActive *				Source;
			CArray<CActiveEvent *>	Nested;
			CArray<CActiveEvent *>	Related;
			CActiveArgs *			Args;

			CActiveEvent * AddNested(CActiveEvent * e)
			{
				e->Parent = this;
				Nested.push_back(e);
				return e;
			}

			CActiveEvent * AddRelated(CActiveEvent * e)
			{
				Related.push_back(e);
				return e;
			}

			CActiveEvent(CActiveEvent && a)
			{
				Type		= a.Type;
				Parent		= a.Parent;
				Source		= a.Source;		a.Source = null;
				Receiver	= a.Receiver;	a.Receiver = null;
				Args		= a.Args;		a.Args = null;

				Nested.reserve(8);
				Related.reserve(8);

			}

			CActiveEvent(const CActiveEvent & a) :	Type(a.Type), 
													Parent(a.Parent),
													Receiver((CActive *)a.Receiver->Take()), 
													Source((CActive *)a.Source->Take()), 
													Args(a.Args ? (CActiveArgs *)(const_cast<CActiveEvent &>(a).Args->Take()) : null)
			{
				Nested.reserve(8);
				Related.reserve(8);
			}


			CActiveEvent(EActiveEvent t, CActive * r, CActive * s, CActiveArgs * a) :	Type(t), 
																						Parent(null),
																						Receiver(r ? (CActive *)r->Take() : null), 
																						Source(s ? (CActive *)s->Take() : null), 
																						Args(a ? (CActiveArgs *)a->Take() : null)
			{
				Nested.reserve(8);
				Related.reserve(8);
			} 


			~CActiveEvent()
			{
				if(Args)
					Args->Free();

				if(Source)
					Source->Free();

				if(Receiver)
					Receiver->Free();
			}
	};

	class UOS_ENGINE_LINKING CActiveGraph : public CEngineEntity
	{
		public:
			CString										Name;
			CActive *									Focus;
			CActive *									HoverFocus;
			CActive *									Root;
			CNodeCapture								Capture;
			CMap<CTouch *, CPick>						Picks;
			CDiagnostic *								Diagnostic;
			CDiagGrid									DiagGrid;
			CActiveEvent								RootEvent;

			CArray<CActiveEvent>						Events;
			
			CMap<CActiveSpace *, CRefList<CActive *>>	Spaces;

			#ifdef _DEBUG
			CMap<CActiveSpace *, CList<CPick>>			_Intersections;
			#endif

			UOS_RTTI
			CActiveGraph(CEngineLevel * l, const CString & name);
			~CActiveGraph();

			bool										Belongs(CActiveSpace * s, CActive * root, CActive * v);
			bool										IsRoot(CActive * v);

			void										AddNode(CActiveSpace * s, CActive * v);
			void										RemoveNode(CActiveSpace * s, CActive * v);
			void										RemoveSpace(CActiveSpace * s);
			
			void										Pick(CScreen * sc, CFloat2 & sp, CActiveSpace * s, CPick * cis, CPick * nis);
			void										Pick(CScreen * sc, CFloat2 & sp, CActiveSpace * as, CActive * root, CActive * n, CCamera * c, CFloat2 & vpp, CRay & r, CPick * cis, CPick * nis);
			void										Pick(CActiveSpace * as, CActive * n, CCamera * c, CFloat2 & vpp, CRay & r, CPick * nis);

			CPick										ReversePick(CCamera * c, CActiveSpace * s, CActive * a, CFloat3 & p);

			CActiveEvent *								Fire(CActiveEvent * pevent, CPick & pk, CInputMessage & m, EGraphEvent type, CActive * n, bool related);
			void										On(CActiveEvent * pevent, CPick & pk, CInputMessage & m);
			void										Off(CActiveEvent * pevent, CPick & pk, CInputMessage & m);
			void										MovePrimary(CActiveEvent * pevent, CPick & pk, CInputMessage & m);
			void										ProcessMouse(CInputMessage & m, CPick & pk);
			void										ProcessKeyboard(CInputMessage & m);
			void										ProcessTouch(CInputMessage & m, CMap<CTouch *, CPick> & pks);
		
			void										PropagateState(CActiveEvent * pevent, CActive * n, CActiveStateArgs * a, CActive * stopat);
			CActiveEvent *								Propagate(EActiveEvent e, CActiveEvent * pevent, CActive * n, CActive * s, CInputArgs * a, EGraphEvent type, bool related); 
			//CActiveEvent *							Propagate(EActiveEvent e, CActiveEvent * pevent, CActive * n, CActive * s, CActiveArgs * a, bool related);

			void										OnNodeDetach(CActive * removed);
			
			CActiveEvent *								AllocateEvent(EActiveEvent t, CActive * r, CActive * s, CActiveArgs * a);
			void										CallEvents();
			void										CallEvent(CActiveEvent * e, bool filtering);

			CActive *									Activate(CActive * newNode, CInputMessage * m);
			
			void										OnDiagnosticsUpdate(CDiagnosticUpdate & a);


	};
}
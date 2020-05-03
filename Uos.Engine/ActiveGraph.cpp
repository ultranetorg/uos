#include "StdAfx.h"
#include "ActiveGraph.h"
#include "ScreenViewport.h"

using namespace uos;

CActiveGraph::CActiveGraph(CEngineLevel * l, const CString & name) : CEngineEntity(l), RootEvent(EActiveEvent::Null, null, null, null)
{
	Name	= name;
		
	Root	= new CActive(l, L"Root");
	Root->Graph = this;
	Root->FinalMatrix = CMatrix::Identity;
	//Root->Space = new CActiveSpace(Level, this, name);

	Events.reserve(128);

	Focus				= null;
	HoverFocus			= null;
	Capture.Pick		= {};

	Diagnostic = Level->Core->Supervisor->CreateDiagnostics(Name + L" - ActiveGraph");
	Diagnostic->Updating += ThisHandler(OnDiagnosticsUpdate);

	DiagGrid.AddColumn(L"Name");
	DiagGrid.AddColumn(L"Space");
	DiagGrid.AddColumn(L"Refs");
	DiagGrid.AddColumn(L"Status");
	DiagGrid.AddColumn(L"S/F/P/L");
#ifdef _DEBUG
	DiagGrid.AddColumn(L"Position");
	DiagGrid.AddColumn(L"Rotation");
	DiagGrid.AddColumn(L"Scale");
#endif
	DiagGrid.AddColumn(L"Mesh BB");
}

CActiveGraph::~CActiveGraph()
{
	if(HoverFocus)
		HoverFocus->Free();

	if(Focus)
		Focus->Free();

	Root->Free();

	Diagnostic->Updating -= ThisHandler(OnDiagnosticsUpdate);
}
	
void CActiveGraph::OnDiagnosticsUpdate(CDiagnosticUpdate & u)
{
	DiagGrid.Clear();

#ifdef _DEBUG
	for(auto & s : _Intersections)
	{
		for(auto i : s.second)
		{
			if(i.Active)
				Diagnostic->Add(L"Viewport: %p   Space: %10s   D: %15f   Z: %.15f   P: [%s]   %-100s", i.Camera->Viewport, s.first->Name, i.Distance, i.Z, i.Point.ToNiceString(), i.Active->FullName);
			else
				Diagnostic->Add(L"Viewport: %p   Space: %10s", i.Camera->Viewport, s.first->Name);
		}
	}
	Diagnostic->Add(L"");
#endif	

	std::function<void(CActiveSpace *, CActive *, const CString &)> 
	
	dump = [this, &dump, &u](auto s, auto n, auto & tab)
	{
		if(Diagnostic->ShouldProceed(u, DiagGrid.GetSize()))
		{
			auto & r = DiagGrid.AddRow();

			if(Diagnostic->ShouldFill(u, DiagGrid.GetSize()))
			{
				r.SetNext(tab + n->Name);
				r.SetNext(s->Name);
				r.SetNext(CInt32::ToString(n->GetRefs()));
				r.SetNext(n->GetStatus());
				r.SetNext(CString(1, ToString(n->State)[0]) + 
								(Focus && n == Focus ? L" AF" : L"   ") + 
								(HoverFocus && n == HoverFocus ? L" HF" : L"   ") + 
								(n->IsPropagator ? L" P" : L"  ") + 
								(n->IsListener ? L" L" : L"  "));
				#ifdef _DEBUG
				r.SetNext(n->_Decomposed.Position.ToNiceString());
				r.SetNext(n->_Decomposed.Rotation.ToNiceString());
				r.SetNext(n->_Decomposed.Scale.ToNiceString());
				#endif																				
				r.SetNext(n->Mesh ? CFloat::NiceFormat(n->Mesh->BBox.GetWidth()) + L" " + CFloat::NiceFormat(n->Mesh->BBox.GetHeight()) + L" " + CFloat::NiceFormat(n->Mesh->BBox.GetDepth()) : L"");
			}

			for(auto i : n->Nodes)
			{
				dump(s, i, tab + L"  ");
			}
		}
	};
	
	for(auto & s : Spaces)
	{
		for(auto i : s.second)
		{
			dump(s.first, i, L"");
		}
	}
	Diagnostic->Add(u, DiagGrid);
	Diagnostic->Add(L"");
}


void CActiveGraph::AddNode(CActiveSpace * s, CActive * v)
{
	Spaces[s].Add(v);
}

void CActiveGraph::RemoveNode(CActiveSpace * s, CActive * v)
{
	Spaces(s).Remove(v);
}

void CActiveGraph::RemoveSpace(CActiveSpace * s)
{
	#ifdef _DEBUG
	if(_Intersections.Contains(s))
		_Intersections.Remove(s);
	#endif

	Spaces.Remove(s);
}

void CActiveGraph::OnNodeDetach(CActive * n)
{
	if(Focus && Focus->HasAncestor(n))
	{
		Activate(n->Parent, null);
	}
}

CActive * CActiveGraph::Activate(CActive * f, CInputMessage * im)
{
	if(Focus != f)
	{
		if(f)
		{
			f = f->GetElderEnabled();
		}

		while(f && f->Enabled && f->ActivatePassOn && !f->Nodes.Empty()) // find descedant with index=0
		{
			f = f->Nodes.Min([](auto a, auto b){ return a->Index < b->Index; });
		}
					
		if(Focus)
		{
			CActive * ca = f->FindCommonAncestor(Focus);

			auto a  = new CActiveStateArgs();
			a->Old	= EActiveState::Active;
			a->New	= EActiveState::Normal;

			auto n = Focus;
				
			while(n != ca)
			{
				n->State = EActiveState::Normal;
				PropagateState(&RootEvent, Root, a, n);
				n = n->Parent;
			}

			a->Free();
		}
		
		if(f)
		{
			CActive * ca = f->FindCommonAncestor(Focus);

			auto a  = new CActiveStateArgs();
			a->Old		= EActiveState::Normal;
			a->New		= EActiveState::Active;

			auto n = ca ? ca->FindChildContaning(f) : Root;
			
			while(n && n->Parent != f)
			{
				n->State = EActiveState::Active;
				PropagateState(&RootEvent, Root, a, n);
				n = n->FindChildContaning(f);
			}
			
			Focus = sh_assign(Focus, f);

			a->Free();

		}
		else
			sh_free(Focus);
	}
	return Focus;
}

CActiveEvent * CActiveGraph::Fire(CActiveEvent * pevent, CPick & pk, CInputMessage & m, EGraphEvent type, CActive * n, bool related)
{
	CInputArgs * args = null;
	EActiveEvent e; 

	if(m.Class == EInputClass::Mouse)
	{
		e = EActiveEvent::Mouse;

		auto a = new CMouseArgs(m);
		a->Pick		= pk;
		a->Event	= type;
	
		if(Capture.Status == 2)
			a->Capture = Capture;

		args = a;
	}
	if(m.Class == EInputClass::TouchScreen)
	{
		e = EActiveEvent::Touch;

		auto a = new CTouchArgs(m);
		a->Picks	= Picks;
		a->Event	= type;
	
		if(Capture.Status == 2)
			a->Capture = Capture;

		args = a;
	}
	if(m.Class == EInputClass::Keyboard)
	{
		e = EActiveEvent::Keyboard;

		auto a = new CKeyboardArgs(m);

		args = a;
	}

	auto ee = Propagate(e, pevent, Root, n, args, type, related);
	args->Free();

	return ee;
}
		
void CActiveGraph::MovePrimary(CActiveEvent * pevent, CPick & pk, CInputMessage & m)
{
	CInputArgs * args = null;

	if(HoverFocus && HoverFocus != pk.Active) // выхожим из одного, входим в другой
	{
		auto ca = HoverFocus->FindCommonAncestor(pk.Active);
	
		auto n = HoverFocus;
	
		while(n && n != ca)
		{
			Fire(pevent, pk, m, EGraphEvent::Leave, n, true);
						
			n = n->Parent;
		}
	
		HoverFocus = sh_assign(HoverFocus, ca);
		//HoverFocus = ca;
	}

	if(pk.Active && HoverFocus != pk.Active)
	{
		auto ca = HoverFocus ? HoverFocus->FindCommonAncestor(pk.Active) : null;
			
		auto n = ca ? ca->FindChildContaning(pk.Active) : Root;
	
		while(n)
		{
			//Level->Log->ReportMessage(this, L"CursorMoved (Enter) : %s", n->FullName.c_str());
			Fire(pevent, pk, m, EGraphEvent::Enter, n, true);
	
			n = n->FindChildContaning(pk.Active);
		}
			
		HoverFocus = sh_assign(HoverFocus, pk.Active);
	}
			
	if(HoverFocus)
	{
		//Level->Log->ReportMessage(this, L"CursorMoved (Hover) : %s", HoverFocus->FullName.c_str());
		auto t = EGraphEvent::Hover;
	
		if(Capture.Status == 2)
		{
			if(pk.Active != Capture.Pick.Active)
			{
				t = EGraphEvent::GuestHover;
			}
		}

		Fire(pevent, pk, m, t, HoverFocus, true);
	}
		
	if(Focus && Capture.Pick.Active)
	{
		if(Capture.Status == 1 && (Capture.Pick.Vpp - pk.Vpp).GetLength() > 3)
		{
			//Level->Log->ReportMessage(this, L"CursorMoved (Captured) : %s", Focus->FullName.c_str());
			Capture.Status = 2;
					
			Fire(pevent, pk, m, EGraphEvent::Captured, Focus, true);
		}

		if(HoverFocus != Focus)
		{
			//Level->Log->ReportMessage(this, L"CursorMoved (Outside) : %s", HoverFocus ?  HoverFocus->FullName.c_str() : L"");
			Fire(pevent, pk, m, EGraphEvent::Roaming, Focus, true);
		}
	}
}

void CActiveGraph::ProcessMouse(CInputMessage & m, CPick & pk)
{
	auto d = dynamic_cast<CMouse *>(m.Device);

	CActiveEvent * e;

	if(m.Action == EInputAction::On)
	{
		e = Fire(&RootEvent, pk, m, EGraphEvent::Input, pk.Active, false);
		On(e, pk, m);
	}
	else
		e = Fire(&RootEvent, pk, m, EGraphEvent::Input, Focus, false);

	if(m.Action == EInputAction::Move)
	{
		MovePrimary(e, pk, m);
	}

	if(m.Action == EInputAction::Rotation)
	{
		//Fire(&RootEvent, pk, m, EGraphEvent::Input, pk.Active, false);
	}

	if(m.Action == EInputAction::Off)
	{
		Off(e, pk, m);
	}

	auto cf = Capture.IsCaptured() ? Focus : HoverFocus;
	
	if(cf)
	{
		d->ResetImage();
		Fire(e, pk, m, EGraphEvent::Feedback, cf, false);
	}

	CallEvents();

	d->ApplyCursor();
}

void CActiveGraph::ProcessTouch(CInputMessage & m, CMap<CTouch *, CPick> & pks)
{
	Picks = pks;

	auto v = m.ValuesAs<CTouchInput>();
	CActiveEvent * e = null;

	auto & pk = pks(v->Touch);

	if(v->Action == ETouchAction::Added)
	{
		if(v->Touch->Primary)
		{
			e = Fire(&RootEvent, pk, m, EGraphEvent::Input, pk.Active, false);
			On(e, pk, m);
		}
	}

	if(!e)
	{
		e = Fire(&RootEvent, pk, m, EGraphEvent::Input, Focus, false);
	}

	if(v->Action == ETouchAction::Movement)
	{
		if(v->Touch->Primary)
		{
			MovePrimary(e, pk, m);
		}
	}

	if(v->Action == ETouchAction::Removed)
	{
		if(v->Touch->Primary)
		{
			Off(e, pk, m);
		}
	}

	CallEvents();

}

void CActiveGraph::ProcessKeyboard(CInputMessage & m)
{
	if(Focus)
	{
		Fire(&RootEvent, CPick(), m, EGraphEvent::Input, Focus, false);
	}

	CallEvents();
}

void CActiveGraph::On(CActiveEvent * pevent, CPick & pk, CInputMessage & m)
{
	if(Activate(pk.Active, &m) != null)
	{
		Capture.Pick	= pk;
		Capture.Message	= m;
		Capture.Status = 1;
	}
}

void CActiveGraph::Off(CActiveEvent * pevent, CPick & pk, CInputMessage & m)
{
	if(pk.Active && Focus)
	{
		auto ca = Focus->FindCommonAncestor(pk.Active);

		if(ca)
		{
			Fire(pevent, pk, m, EGraphEvent::Click, ca, true);
		}
	}

	if(Capture.Pick.Active)
	{
		if(Capture.Status == 2)
		{
			Fire(pevent, pk, m, EGraphEvent::Released, Focus, true);
		}

		Capture.Pick.Active->Free();
		Capture.Pick.Active = null;
		Capture.Status = 0;
	}
}

CPick CActiveGraph::ReversePick(CCamera * c, CActiveSpace * s, CActive * a, CFloat3 & p)
{
	CPick pk;

	pk.Vpp = c->ProjectVertexXY(p.VertexTransform(a->FinalMatrix * s->Matrix));
	pk.Active = a; a->Take();
	pk.Space = s; s->Take();
	pk.Camera = c;
	pk.Ray = c->Raycast(pk.Vpp);
	pk.Point = p;

	return pk;
}
	
void CActiveGraph::Pick(CScreen * sc, CFloat2 & sp, CActiveSpace * s, CPick * cpk, CPick * npk)
{
	if(Spaces.Contains(s))
	{
		for(auto & i : Spaces(s))
		{
			Pick(sc, sp, s, i, i, null, CFloat2(NAN, NAN), CRay(), cpk, npk);
		}
	}
}

void CActiveGraph::Pick(CScreen * sc, CFloat2 & sp, CActiveSpace * s, CActive * root, CActive * a, CCamera * cam, CFloat2 & vpp, CRay & ray, CPick * cis, CPick * nis)
{
	if(!Belongs(s, root, a))
	{
		return;
	}

	if(a->Enabled)
	{
		if(s->View)
		{
			for(auto c : s->View->Cameras)
			{
				if(auto svp = c->Viewport->As<CScreenViewport>())
				{
					if(svp->Target->Screen != sc)
					{
						continue;
					}
				}

				if(c->Viewport->Contains(sp))
				{
					auto p = c->Viewport->ScreenToViewport(sp); // separate var important! do not use cis members
					auto r = c->Raycast(p); // separate var important! do not use cis members

					//cis->Space	= s; s->Take();
					cis->Camera	= c;
					cis->Vpp	= p;
					cis->Ray	= r;
					
					Pick(s, a, c, p, r, nis);
					
					for(auto i : a->Nodes)
					{
						Pick(sc, sp, s, root, i, c, p, r, cis, nis);
					}
				}
			}
		} 
		else
		{
			Pick(s, a, cam, vpp, ray, nis);

			for(auto i : a->Nodes)
			{
				Pick(sc, sp, s, root, i, cam, vpp, ray, cis, nis);
			}
		}
	}
}

void CActiveGraph::Pick(CActiveSpace * s, CActive * n, CCamera * c, CFloat2 & vpp, CRay & r, CPick * nis)
{
	if(n->Enabled && n->IsReady())
	{
		auto is = n->Mesh->Intersect(r.Transform(!(n->FinalMatrix * s->Matrix)));
				
		if(is.StartIndex != -1)
		{
			auto cl = n->GetActualClipper();

			while(cl)
			{
				if(cl->ClippingMesh->Intersect(r.Transform(!(cl->FinalMatrix * s->Matrix))).StartIndex == -1)
				{
					is.StartIndex = -1;
					break;
				}
				cl = cl->GetActualClipper();
			}
		}
		
		if(is.StartIndex != -1)
		{
			auto z = c->ProjectVertex((n->FinalMatrix * s->Matrix).TransformCoord(is.Point /*CPick::GetPosition(n, is)*/)).z;

			//if(z < nis->Z)
			if(is.Distance < nis->Distance)
			{
				nis->Camera		= c;
				nis->Vpp		= vpp;
				nis->Ray		= r;
	
				nis->Space		= sh_assign(nis->Space, s);
				nis->Active		= sh_assign(nis->Active, n);
				nis->Mesh		= sh_assign(nis->Mesh, n->Mesh);
				nis->StartIndex	= is.StartIndex;
				nis->Point		= is.Point;
				nis->Distance	= is.Distance;
				nis->Z			= z;
			}

			#ifdef _DEBUG
				CPick _is;
				_is.Camera		= c;
				_is.Vpp			= vpp;
				_is.Ray			= r;

				_is.Space		= sh_assign(_is.Space, s);
				_is.Active		= sh_assign(_is.Active, n);
				_is.Mesh		= sh_assign(_is.Mesh, n->Mesh);
				_is.StartIndex	= is.StartIndex;
				_is.Point		= is.Point;
				_is.Distance	= is.Distance;
				_is.Z			= z;

				_Intersections[s].push_back(_is);
			#endif		
		}
	}
}

CActiveEvent * CActiveGraph::AllocateEvent(EActiveEvent t, CActive * r, CActive * s, CActiveArgs * a)
{
	Events.push_back(CActiveEvent(t, r, s, a));
	return &Events.back();
}

void CActiveGraph::CallEvents()
{
	for(auto i = 0u; i< RootEvent.Nested.size(); i++) // do not foreach! 
	{
		CallEvent(RootEvent.Nested[i], true);
	}

	RootEvent.Nested.clear();
	RootEvent.Related.clear();

	Events.clear();
} 

void CActiveGraph::CallEvent(CActiveEvent * e, bool filtering)
{
	//#ifdef _DEBUG
	//	auto mia = 	dynamic_cast<CMouseArgs *>(e->Args);
	//		if(e->Type == EActiveEvent::Move && mia && mia->Action == EInputAction::Rotation)
	//		{
	//			Level->Log->ReportDebug(this, L"%s   %4d     %-50s     %s", filtering ? L"F" : L" ", dynamic_cast<CMouseArgs *>(e->Args)->Action,  e->Receiver->FullName,  e->Source->FullName);
	//		}
	//#endif

	if(filtering)
	{
		if(e->Receiver->HasAncestor(Root))
		{
			switch(e->Type)
			{
				case EActiveEvent::Mouse:		e->Receiver->MouseFilter	(e->Receiver, e->Source, (CMouseArgs *)e->Args);	break;
				case EActiveEvent::Keyboard:	e->Receiver->KeyboardFilter	(e->Receiver, e->Source, (CKeyboardArgs *)e->Args);	break;
				case EActiveEvent::Touch:		e->Receiver->TouchFilter	(e->Receiver, e->Source, (CTouchArgs *)e->Args);	break;
			}
		}
	} 
	else
	{
		if(e->Receiver->HasAncestor(Root))
		{
			switch(e->Type)
			{
				case EActiveEvent::State:		e->Receiver->StateChanged	(e->Receiver, e->Source, (CActiveStateArgs *)e->Args);	break;

				case EActiveEvent::Mouse:		e->Receiver->MouseInput		(e->Receiver, e->Source, (CMouseArgs *)e->Args);	break;
				case EActiveEvent::Keyboard:	e->Receiver->KeyboardInput	(e->Receiver, e->Source, (CKeyboardArgs *)e->Args);	break;
				case EActiveEvent::Touch:		e->Receiver->TouchInput		(e->Receiver, e->Source, (CTouchArgs *)e->Args);	break;
			}
		}
	}
	
	if(e->Type == EActiveEvent::State)
	{

		if(!e->Args->StopPropagation)
		{
			for(auto & i : e->Nested)
			{
				CallEvent(i, false);
			}
		} 
	} 
	else
	{
		if(filtering)
		{
			if(!e->Nested.empty() && !e->Args->StopPropagation)
			{
				for(auto & i : e->Nested)
				{
					CallEvent(i, true);
				}
			} 
			else
			{
				CallEvent(e, false);
			} 

			if(!e->Args->StopRelatedPropagation)
			{
				for(auto & i : e->Related)
				{
					CallEvent(i, true);
				}
			}

		}
		else
		{
			if(e->Parent && e->Parent != &RootEvent && !e->Args->StopPropagation)
			{
				CallEvent(e->Parent, false);
			} 
			// else
			//	 the end 
		}
	}

}

CActiveEvent * CActiveGraph::Propagate(EActiveEvent e, CActiveEvent * pevent, CActive * n, CActive * s, CInputArgs * a, EGraphEvent type, bool related) // propagate down
{
	if(s == n || n->IsListener)
	{
		pevent = !related ? pevent->AddNested(AllocateEvent(e, n, s, a)) : pevent->AddRelated(AllocateEvent(e, n, s, a));
	}

	if(type == EGraphEvent::Enter)
	{
		if(n->Parent)
		{
			n->Parent->HoverChild = n;
		}
	}
	if(type == EGraphEvent::Leave)
	{
		if(n->Parent)
		{
			n->Parent->HoverChild = null;
		}
	}

	//#ifdef _DEBUG
	//	if(a->Action == EInputAction::Rotation)
	//	{
	//		Level->Log->ReportDebug(this, L"%s", n->Name);
	//	}
	//#endif // _DEBUG

	auto c = n->FindChildContaning(s);

	if(n->IsPropagator && c)
	{
		Propagate(e, pevent, c, s, a, type, false);
	}

	return pevent;
}

void CActiveGraph::PropagateState(CActiveEvent * pevent,  CActive * n, CActiveStateArgs * a, CActive * last) // downwards recursively
{
	pevent = pevent->AddNested(AllocateEvent(EActiveEvent::State, n, last, a));

	if(n == last)
	{
		return;
	}

	auto c = n->FindChildContaning(last);

	if(c)
	{
		PropagateState(pevent, c, a, last);
	}
}

bool CActiveGraph::Belongs(CActiveSpace * s, CActive * root, CActive * v)
{
	auto & roots = Spaces(s);

	if(!roots.Contains(root))
	{
		throw CException(HERE, L"Root <-> VisualSpace error");
	}

	auto p = v;

	while(p && !IsRoot(p))
	{
		p = p->Parent;
	}

	return p == root;
}

bool CActiveGraph::IsRoot(CActive * v)
{
	for(auto & i : Spaces)
	{
		if(i.second.Contains(v))
		{
			return true;
		}
	}

	return false;
}

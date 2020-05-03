#include "StdAfx.h"
#include "Interactor.h"

using namespace uos;

CInteractor::CInteractor(CEngineLevel * l, CScreenEngine * ve, CInputSystem * ie) : CEngineEntity(l), Pc(L"Interactor")
{
	Level = new CEngineLevel(*l);
	Level->Log	= Level->Core->Supervisor->CreateLog(L"Interactor");

	ViewEngine	= ve;
	InputEngine	= ie;

	InputEngine->Recieved += ThisHandler(Process);
		
	Diagnostics = Level->Core->Supervisor->CreateDiagnostics(L"Interactor");
	Diagnostics->Updating	+= ThisHandler(OnDiagnosticsUpdate);

	Level->Core->AddPerformanceCounter(&Pc);
}
	
CInteractor::~CInteractor()
{
	InputEngine->Recieved -= ThisHandler(Process);

	Level->Core->RemovePerformanceCounter(&Pc);
	delete Level;

	Diagnostics->Updating -= ThisHandler(OnDiagnosticsUpdate);
}

void CInteractor::OnDiagnosticsUpdate(CDiagnosticUpdate & a)
{
	Diagnostics->Add(L"Update: %f", Pc.GetTime());
}

CActiveGraph * CInteractor::CreateActiveGraph(const CString & name)
{
	return new CActiveGraph(Level, name);
}

CActiveLayer * CInteractor::AddLayer(CScreenViewport * screen, CActiveSpace * space)
{
	auto rt = new CActiveLayer(Level);
	rt->Viewport = screen;
	rt->Space = space;
	Layers.push_back(rt);
	return rt;
}

void CInteractor::RemoveTarget(CActiveLayer * t)
{
	delete t;
	Layers.Remove(t);
}

void CInteractor::Pick(CScreen * sc, CFloat2 & sp, CActiveSpace * s, CPick * cis, CPick * nis)
{
	CPick c, n;
	
	for(auto i : s->Spaces)
	{
		Pick(sc, sp, i, cis, nis);

		if(nis->Active)
		{
			return;
		}
	}

	for(auto g : s->Graphs)
	{
		g->Pick(sc, sp, s, &c, &n);
	
		if(!cis->Camera && c.Camera)
		{
			*cis = c;
		}
		if(n.Active)
		{
			*nis = n; 
			return;
		}
	}
}

void CInteractor::Pick(CScreen * s, CFloat2 & p, CPick * pick)
{
	CActiveGraph * graph = null;
	CPick cpick, npic;

	#ifdef _DEBUG
	for(auto i : Layers)
	{
		std::function<void(CActiveSpace *)> f;
		f = [&f](auto s)
			{
				for(auto i : s->Graphs)
					i->_Intersections.clear();
	
				for(auto i : s->Spaces)
					f(i);
			};

		f(i->Space);
	}
	#endif
	
	
	for(auto i : Layers)
	{
		if(i->Viewport->Target->Screen == s)
		{
			Pick(s, p, i->Space, &cpick, &npic);
	
			*pick = npic.Active ? npic : cpick;
							
			if(pick->Active)
			{
				break;;
			}
		}
	}

}

void CInteractor::Process(CInputMessage & m)
{
	CPick pick;
	CActiveGraph * graph = null;

	Pc.BeginMeasure();

	if(m.Class == EInputClass::Mouse)
	{
		auto p = m.ValuesAs<CMouseInput>()->Position;

		Pick(m.Screen, p, &pick);
	
		if(pick.Active)
		{
			auto g = pick.Active->Graph;
			
			g->ProcessMouse(m, pick);

			if(g->Focus)
			{
				FocusGraph = g;
			}
		}
	}

	if(m.Class == EInputClass::TouchScreen)
	{
		auto v = m.ValuesAs<CTouchInput>();

		CMap<CTouch *, CPick> pks;

		for(auto i : *m.ValuesAs<CTouchInput>()->Touches)
		{
			Pick(m.Screen, i->Position, &pks[i]);
		}

		if(pks(v->Touch).Active)
		{
			auto g = pks(v->Touch).Active->Graph;
				
			g->ProcessTouch(m, pks);
	
			//#ifdef _DEBUG
			//Level->Log->ReportDebug(this, L"%s", pks(v->Touch).Space->Name);
			//#endif 
	
			if(g->Focus)
			{
				FocusGraph = g;
			}
		}
	}

	if(m.Class == EInputClass::Keyboard)
	{
		if(FocusGraph)
		{
			FocusGraph->ProcessKeyboard(m);
		}
	}

	MessageProcessed(m);

	Pc.EndMeasure();
}

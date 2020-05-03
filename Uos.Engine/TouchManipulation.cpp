#include "stdafx.h"
#include "TouchManipulation.h"
#include "InputSystem.h"
#include <manipulations_i.c>

using namespace uos;

CTouchManipulation::CTouchManipulation(CInputSystem * ie, CInputDevice * id, IManipulationProcessor * mp, CScreen * sc)
{
	InputEngine = ie;
	Device = id;
	Screen = sc;
	ManipulationProcessor = mp;

	//Set initial ref count to 1.
	m_cRefCount = 1;

	ManipulationProcessor->put_PivotRadius(-1);

	m_cStartedEventCount = 0;
	m_cDeltaEventCount = 0;
	m_cCompletedEventCount = 0;

	HRESULT hr = S_OK;

	//Get the container with the connection points.
	IConnectionPointContainer * spConnectionContainer;

	hr = mp->QueryInterface(IID_IConnectionPointContainer, (LPVOID *)&spConnectionContainer);
	//hr = manip->QueryInterface(&spConnectionContainer);

	if(spConnectionContainer == NULL)
	{
		// something went wrong, try to gracefully quit
	}

	//Get a connection point.
	hr = spConnectionContainer->FindConnectionPoint(__uuidof(_IManipulationEvents), &m_pConnPoint);

	if(m_pConnPoint == NULL)
	{
		// something went wrong, try to gracefully quit
	}

	DWORD dwCookie;

	//Advise.
	hr = m_pConnPoint->Advise(this, &dwCookie);
}

int CTouchManipulation::GetStartedEventCount()
{
	return m_cStartedEventCount;
}

int CTouchManipulation::GetDeltaEventCount()
{
	return m_cDeltaEventCount;
}

int CTouchManipulation::GetCompletedEventCount()
{
	return m_cCompletedEventCount;
}

double CTouchManipulation::GetX()
{
	return m_fX;
}

double CTouchManipulation::GetY()
{
	return m_fY;
}

CTouchManipulation::~CTouchManipulation()
{
	//Cleanup.
}

///////////////////////////////////
//Implement IManipulationEvents
///////////////////////////////////

HRESULT STDMETHODCALLTYPE CTouchManipulation::ManipulationStarted(FLOAT x, FLOAT y)
{
	m_cStartedEventCount++;

	CInputMessage m;
	m.Class		= EInputClass::TouchScreen;
	m.Control	= EControl::Screen;
	m.Action	= EInputAction::On;
	m.Id		= InputEngine->GetNextID();
	m.Screen	= Screen;
	m.Device	= Device;

	auto v = new CTouchInput();
	m.Values = v;

	//v->Stage = EMoveStage::Start; 
	//v->Position = Screen->NativeToScreen(CFloat2(x/100, y/100));

	InputEngine->SendInput(m);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTouchManipulation::ManipulationDelta(float x,
																float y,
																float translationDeltaX,
																float translationDeltaY,
																float scaleDelta,
																float expansionDelta,
																float rotationDelta,
																float cumulativeTranslationX,
																float cumulativeTranslationY,
																float cumulativeScale,
																float cumulativeExpansion,
																float cumulativeRotation)
{

	CInputMessage m;
	m.Class		= EInputClass::TouchScreen;
	m.Control	= EControl::Screen;
	m.Action	= EInputAction::Move;
	m.Id		= InputEngine->GetNextID();
	m.Screen	= Screen;
	m.Device	= Device;
	m.Values	= m.MakeValues<CTouchInput>();
	
	auto v = m.ValuesAs<CTouchInput>();
	
	x						/= 100.f; // * InputEngine->DisplayHardware->Scaling.x;
	y						/= 100.f; // * InputEngine->DisplayHardware->Scaling.y;
	translationDeltaX		/= 100.f; // * InputEngine->DisplayHardware->Scaling.x;
	translationDeltaY		/= 100.f; // * InputEngine->DisplayHardware->Scaling.y;
	scaleDelta				/= 100.f; // * InputEngine->DisplayHardware->Scaling.x;
	expansionDelta			/= 100.f; // * InputEngine->DisplayHardware->Scaling.x;
	rotationDelta			/= 100.f; // * InputEngine->DisplayHardware->Scaling.x;
	cumulativeTranslationX	/= 100.f; // * InputEngine->DisplayHardware->Scaling.x;
	cumulativeTranslationY	/= 100.f; // * InputEngine->DisplayHardware->Scaling.y;
	cumulativeScale			/= 100.f; // * InputEngine->DisplayHardware->Scaling.x;
	cumulativeExpansion		/= 100.f; // * InputEngine->DisplayHardware->Scaling.x;
	cumulativeRotation		/= 100.f; // * InputEngine->DisplayHardware->Scaling.x;

	//v->Stage = EMoveStage::Move; 
	//v->Position				= Screen->NativeToScreen(CFloat2(x, y));
	//v->TranslationDelta		= {translationDeltaX,		translationDeltaY};
	//v->ScaleDelta			= {scaleDelta,				scaleDelta};
	//v->ExpansionDelta		= {expansionDelta,			expansionDelta};
	//v->RotationDelta		= {rotationDelta,			rotationDelta};
	//v->CumulativeTranslation= {cumulativeTranslationX,	cumulativeTranslationY};
	//v->CumulativeScale		= {cumulativeScale,			cumulativeScale};
	//v->CumulativeExpansion	= {cumulativeExpansion,		cumulativeExpansion};
	//v->CumulativeRotation	= {cumulativeRotation,		cumulativeRotation};


	InputEngine->SendInput(m);


//#ifdef _DEBUG
//	InputEngine->Level->Log->ReportDebug(InputEngine,	L"%g %g   %g %g   %g %g %g   %g %g %g %g %g",
//														x,
//														y,
//														translationDeltaX,
//														translationDeltaY,
//														scaleDelta,
//														expansionDelta,
//														rotationDelta,
//														cumulativeTranslationX,
//														cumulativeTranslationY,
//														cumulativeScale,
//														cumulativeExpansion,
//														cumulativeRotation);
//#endif // _DEBUG

	///m_cDeltaEventCount++;
	///
	///RECT rect;
	///
	///GetWindowRect(m_hWnd, &rect);
	///
	///int oldWidth = rect.right-rect.left;
	///int oldHeight = rect.bottom-rect.top;
	///
	///// scale and translate the window size / position    
	///MoveWindow(m_hWnd,                                                  // the window to move
	///		   static_cast<int>(rect.left + (translationDeltaX / 100.0f)), // the x position
	///		   static_cast<int>(rect.top + (translationDeltaY / 100.0f)),  // the y position
	///		   static_cast<int>(oldWidth * scaleDelta),                    // width
	///		   static_cast<int>(oldHeight * scaleDelta),                   // height
	///		   TRUE);                                                      // redraw


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTouchManipulation::ManipulationCompleted(float x,
																	float y,
																	float cumulativeTranslationX,
																	float cumulativeTranslationY,
																	float cumulativeScale,
																	float cumulativeExpansion,
																	float cumulativeRotation)
{
	m_cCompletedEventCount++;

	//m_fX = x;
	//m_fY = y;
	//
	//// place your code handler here to do any operations based on the manipulation   

	CInputMessage m;
	m.Class		= EInputClass::TouchScreen;
	m.Control	= EControl::Screen;
	m.Action	= EInputAction::Off;
	m.Id		= InputEngine->GetNextID();
	m.Screen	= Screen;
	m.Device	= Device;
	m.Values	= m.MakeValues<CTouchInput>();
	
	auto v = m.ValuesAs<CTouchInput>();

	x						/= 100.f;// * InputEngine->DisplayHardware->Scaling.x;
	y						/= 100.f;// * InputEngine->DisplayHardware->Scaling.y;
	cumulativeTranslationX	/= 100.f;// * InputEngine->DisplayHardware->Scaling.x;
	cumulativeTranslationY	/= 100.f;// * InputEngine->DisplayHardware->Scaling.y;
	cumulativeScale			/= 100.f;// * InputEngine->DisplayHardware->Scaling.x;
	cumulativeExpansion		/= 100.f;// * InputEngine->DisplayHardware->Scaling.x;
	cumulativeRotation		/= 100.f;// * InputEngine->DisplayHardware->Scaling.x;


	//v->Stage = EMoveStage::End; 
	//v->Position					= Screen->NativeToScreen(CFloat2(x, y));
	//v->CumulativeTranslation	= {cumulativeTranslationX,	cumulativeTranslationY};
	//v->CumulativeScale			= {cumulativeScale,			cumulativeScale};
	//v->CumulativeExpansion		= {cumulativeExpansion,		cumulativeExpansion};
	//v->CumulativeRotation		= {cumulativeRotation,		cumulativeRotation};

	InputEngine->SendInput(m);

	return S_OK;
}


/////////////////////////////////
//Implement IUnknown
/////////////////////////////////

ULONG CTouchManipulation::AddRef(void)
{
	return ++m_cRefCount;
}

ULONG CTouchManipulation::Release(void)
{
	m_cRefCount--;

	if(0 == m_cRefCount)
	{
		delete this;
		return 0;
	}

	return m_cRefCount;
}

HRESULT CTouchManipulation::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	if(IID__IManipulationEvents == riid)
	{
		*ppvObj = (_IManipulationEvents *)(this);
		AddRef();
		return S_OK;
	}
	else if(IID_IUnknown == riid)
	{
		*ppvObj = (IUnknown *)(this);
		AddRef();
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}


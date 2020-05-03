#include "stdafx.h"
#include "WorldServer.h"
#include "IMenu.h"
#include "MobileSkinModel.h"
#include "EnvironmentWindow.h"
#include "TextEdit.h"
#include "Button.h"
#include "Stack.h"
#include "Table.h"
#include "DefaultIcon.h"
#include "Logo.h"
#include "VrWorld.h"
#include "DesktopWorld.h"
#include "MobileWorld.h"
#include "GroupIcon.h"
#include "SingleUnit.h"
#include "HighspaceGroupUnit.h"
#include "LowspaceGroupUnit.h"

using namespace uos;

static CWorldServer * This = null;
	
CServer * StartUosServer(CLevel2 * l, CServerInfo * si)
{
	for(auto i : l->Core->FindStartCommands(si->Url))
	{
		if(i.Query.Contains(L"Mode", WORLD_VR_EMULATION))
		{
			This = new CVrWorld(l, si);
		}
		else if(i.Query.Contains(L"Mode", WORLD_MOBILE_EMULATION))
		{
			This = new CMobileWorld(l, si);
		}
	}

	if(!This)
	{
		This = new CDesktopWorld(l, si);
	}

	return This;
}

void StopUosServer()
{
	delete This;
}

CWorldServer::CWorldServer(CLevel2 * l, CServerInfo * si) : CServer(l, si), CWorld(l)//, InteractiveMover(l), BackgroundMover(l)
{
	Server = this;
	Log  = Core->Supervisor->CreateLog(WORLD);
	Nexus->Stopping += ThisHandler(OnNexusStopping);
	Storage = l->Nexus->Storage;

	PfcUpdate	= new CPerformanceCounter(CString(WORLD) + L" update");
	Diagnostic	= Core->Supervisor->CreateDiagnostics(WORLD);
	Diagnostic->Updating += ThisHandler(OnDiagnosticsUpdating);

	DiagGrid.AddColumn(L"Name");
	DiagGrid.AddColumn(L"Class");
	DiagGrid.AddColumn(L"Position");
	DiagGrid.AddColumn(L"Rotation");
	DiagGrid.AddColumn(L"Scale");
	DiagGrid.AddColumn(L"Size");
	DiagGrid.AddColumn(L"Smax");
	DiagGrid.AddColumn(L"Pmax");
	DiagGrid.AddColumn(L"Expressions");

	for(auto i : Storage->Enumerate(MapPath(L""), L"*.*"))
	{
		if(i.Type == CDirectory::GetClassName())
		{
			auto ws = Storage->OpenReadStream(CPath::Join(i.Path, L"World.xon"));
			auto es = Storage->OpenReadStream(CPath::Join(i.Path, L"Engine.xon"));
			Modes.push_back({	
								CPath::GetName(i.Path),
								new CTonDocument(CXonTextReader(ws)), 
								new CTonDocument(CXonTextReader(es)), 
							});
			Storage->Close(ws);
			Storage->Close(es);
		}
	}
}

CWorldServer::~CWorldServer()
{
	while(auto i = Objects.Find([](auto j){ return j->Shared;  }))
	{
		DestroyObject(i);
	}

	if(Sphere)
	{
		Sphere->Free();
	}

	Area->Free();

	for(auto & i : RenderLayers)
		Engine->Renderer->RemoveTarget(i.second);

	for(auto & i : ActiveLayers)
		Engine->Interactor->RemoveTarget(i.second);
	
	delete HudView;
	delete NearView;
	delete MainView;
	delete ThemeView;

	for(auto i : Viewports)
	{
		delete i;
	}
	
	delete ThemeVisualGraph;
	delete MainVisualGraph;
	delete MainActiveGraph;

	delete Materials;
	delete Style;
	delete Engine;
	
	delete EngineConfig;
	delete WorldConfig;
	delete AreasConfig;

	delete PfcUpdate;

	for(auto & i : Modes)
	{
		delete i.WorldConfig;
		delete i.EngineConfig;
	}

	Nexus->Stopping -= ThisHandler(OnNexusStopping);
	Diagnostic->Updating -= ThisHandler(OnDiagnosticsUpdating);
}

IProtocol * CWorldServer::Connect(CString const & pr)
{
	return this;
}

void CWorldServer::Disconnect(IProtocol * o)
{
}

void CWorldServer::Start(EStartMode sm)
{
	Start();
}

void CWorldServer::Start()
{
	ScreenshotId = Core->RegisterGlobalHotKey(MOD_ALT|MOD_CONTROL, VK_SNAPSHOT, [this](auto){ Engine->ScreenEngine->TakeScreenshot(Name); });
	
	auto sconfig = LoadServerDocument(Name + L"/World.xon"); 
	auto gconfig = LoadGlobalDocument(Name + L"/World.xon");
		
	if(gconfig)
	{
		WorldConfig = gconfig;
		delete sconfig;
	}
	else
		WorldConfig = sconfig;

	Layout	= WorldConfig->Get<CString>(L"Layout");
	Fov		= WorldConfig->Get<CFloat>(L"Fov");

	for(auto i : Core->FindStartCommands(Url))
		if(i.Query.Contains(L"Layout"))
		{
			Layout = i.Query(L"Layout"); 
			break;
		}


	if(!(AreasConfig = LoadGlobalDocument(Name + L"/" + Layout + L".layout/Areas.xon")))
	{
		AreasConfig = LoadServerDocument(Name + L"/Areas.xon");
		Initializing = true;
	}
	
	auto df = MapPath(L"Engine.xon");
	auto cf = MapPath(Name + L"/Engine.xon");
	EngineConfig = new CConfig(this, df, cf);
		
	Engine = new CEngine(this, this, EngineConfig);

	Engine->ScreenEngine->SetLayout(Layout);

	Materials = new CMaterialPool(Engine);

	auto sf = Storage->OpenReadStream(MapPath(Name + L"/Default.style"));
	Style = new CStyle(Engine, Materials, CXonTextReader(sf));
	Storage->Close(sf);

	auto arrow = CRectangleSectionMenuItem::CreateArrowMesh(this);
	Style->DefineMesh(L"RectangleSectionMenuItem/Arrow", arrow);
	arrow->Free();

	for(auto i : Engine->ScreenEngine->Screens)
	{
		 Targets.push_back(Engine->DisplaySystem->GetAppropriateDevice(i->As<CWindowScreen>())->AddTarget(i->As<CWindowScreen>()));
	}
				
	InitializeViewports();

	ThemeVisualGraph	= Engine->CreateTreeVisualGraph(L"Theme");
	MainVisualGraph		= Engine->CreateTreeVisualGraph(L"Main");
	MainActiveGraph		= Engine->Interactor->CreateActiveGraph(L"Main");

	MainActiveGraph->Root->Listen(true);

	InitializeGraphs();
	
	InitializeView();
	
	Area = new CArea(this);
	
	for(auto i : Viewports)
	{	
		Area->AllocateVisualSpace(i);
		Area->AllocateActiveSpace(i);
	}

	Area->Load(AreasConfig->One(L"Area"));
	
	ServiceBackArea		= Area->Match(AREA_SERVICE_BACK);
	ThemeArea			= Area->Match(AREA_THEME);
	FieldArea			= Area->Match(AREA_FIELDS)->As<CPositioningArea>();
	MainArea			= Area->Match(AREA_MAIN)->As<CPositioningArea>();
	HudArea				= Area->Match(AREA_HUD)->As<CPositioningArea>();
	TopArea				= Area->Match(AREA_TOP)->As<CPositioningArea>();
	ServiceFrontArea	= Area->Match(AREA_SERVICE_FRONT);

	ServiceBackArea		->SetView(MainView); 
	ThemeArea			->SetView(ThemeView); 
	FieldArea			->SetView(MainView); 
	MainArea			->SetView(MainView); 
	HudArea				->SetView(HudView); 
	TopArea				->SetView(HudView); 
	ServiceFrontArea	->SetView(MainView); 
	
	if(Area->Match(AREA_BACKGROUND))
	{
		BackArea = Area->Match(AREA_BACKGROUND)->As<CPositioningArea>();
		BackArea->SetView(MainView); 
	}

	InitializeAreas();
	
	for(auto vp : Viewports)
	{
		RenderLayers[vp]	= Engine->Renderer->AddLayer(vp, Area->VisualSpaces.Match(vp).Space);
		ActiveLayers[vp]	= Engine->Interactor->AddLayer(vp, Area->ActiveSpaces.Match(vp).Space);
	}
	
	Sphere = new CSphere(this);
	auto sphere = AllocateUnit(Sphere);
	Show(sphere, AREA_SERVICE_BACK, null);

	auto logo = new CLogo(this);
	auto a = AllocateUnit(logo);
	Show(a, AREA_SERVICE_FRONT, null);

	InitializeModels();

	Engine->Renderer->Update();

	for(auto i : AreasConfig->Many(L"Open"))
	{
		auto u = FindUnit(i->Get<CUol>());

		auto sf = new CShowParameters();
		sf->Activate = false;

		OpenUnit(u, AREA_LAST, sf);

		sf->Free();
	}
	
	Engine->Start();

	Hide(a, null);
	logo->Free();

	Starting = false;
}

void CWorldServer::OnNexusStopping()
{
	Engine->Stop();

	Storage->CreateGlobalDirectory(this);
	Storage->CreateLocalDirectory(this);

	for(auto i : AreasConfig->Many(L"Open"))
		AreasConfig->Remove(i);

	AreasConfig->Remove(AreasConfig->One(L"Area"));

	while(auto u = Units.First())
	{
		if(u->Parent && u->Parent->Parent == Area && u->Lifespan == ELifespan::Permanent)
		{
			u->Parent->Remember(u);
			AreasConfig->Add(L"Open")->Set(u->Entity.Url);
		}

		Dealloc(u);
	}

	Area->Save(AreasConfig->Add(L"Area"));

	auto f = Storage->OpenWriteStream(MapUserGlobalPath(Name + L"/" + Layout + L".layout/Areas.xon"));
	AreasConfig->Save(&CXonTextWriter(f, true));
	Storage->Close(f);

	f = Storage->OpenWriteStream(MapUserGlobalPath(Name + L"/World.xon"));
	WorldConfig->Save(&CXonTextWriter(f));
	Storage->Close(f);

	Core->UnregisterGlobalHotKey(ScreenshotId);
}

void CWorldServer::Switch(CString const & mode, CString const & layout)
{
	auto cmd = (CUrl)Url;
	cmd.Query[L"Mode"] = mode;
	cmd.Query[L"Layout"] = layout;
	Core->AddRestartCommand(cmd);
	Core->Exit();
}

CGroup * CWorldServer::CreateGroup(CString const & name)
{
	auto  o = new CGroup(this, name);
	RegisterObject(o, true);
	return o;
}

CNexusObject * CWorldServer::CreateObject(CString const & name)
{	
	CNexusObject * o = null;

	auto type = CUol::GetObjectType(name);

	if(type == CGroup::GetClassName())	o = new CGroup(this, name);

	return o;
}

CUol CWorldServer::GenerateAvatar(CUol & entity, CString const & type)
{
	CList<CUol> avs;

	auto protocol = Nexus->Connect<IAvatarProtocol>(this, entity, AVATAR_PROTOCOL);

	CUol avatar;
	CAvatar * a = null;

	if(protocol)
	{
		avs = protocol->GenerateSupportedAvatars(entity, type);
		if(!avs.empty())
		{
			avatar = avs.front();
		}
	}
	else
	{
		for(auto & i : Level->Nexus->ConnectMany<IAvatarProtocol>(this, AVATAR_PROTOCOL))
		{
			avs = i->GenerateSupportedAvatars(entity, type);
			if(!avs.empty())
			{
				protocol = i;
				avatar = avs.front();
				break;
			}
		}
	}

	if(avatar.IsEmpty())
	{
		auto p = CMap<CString, CString>{{L"entity", entity.ToString()}, {L"type", type}};
		
		if(type == AVATAR_ICON2D)	avatar = CUol(Url, CGuid::Generate64(CDefaultIcon::GetClassName()), p);
	}

	
	return avatar;
}

CAvatar * CWorldServer::CreateAvatar(CUol & avatar, CString const & dir)
{
	CAvatar * a = null;

	auto protocol = Nexus->Connect<IAvatarProtocol>(this, avatar, AVATAR_PROTOCOL);
		
	if(protocol)
	{
		a = protocol->CreateAvatar(avatar);
	
		if(a)
		{
			a->Protocol = protocol;
			///a->Destroying += ThisHandler(OnDependencyDestroying);

			a->SetDirectories(dir);
			a->SetEntity(CUol(avatar.Parameters(L"entity")));

			if(!dir.empty() && a->IsSaved())
			{
				a->Load();
			}
		}
	}
	else
	{	
		a = new CDefaultIcon(this);
		a->Protocol = CProtocolConnection<IAvatarProtocol>(CConnection(this, this, this, AVATAR_PROTOCOL));
		RegisterObject(a, false);
		a->Free();
	}

	return a;
}

void CWorldServer::DestroyAvatar(CAvatar * a)
{
	if(a->Protocol == this)
		Server->DestroyObject(a);
	else
		a->Protocol->DestroyAvatar(a);
}

CUnit * CWorldServer::AllocateUnit(CModel * m)
{
	auto u = new CSingleUnit(this, m, MainView, MainVisualGraph, MainActiveGraph);
	Units.push_back(u);
	return u;
}

CUnit * CWorldServer::AllocateUnit(CUol & entity, CString const & type)
{
	auto dir = MapRelative(Name + L"/" + Layout + L".layout/" + entity.Object);
	
	auto u = (CUnit *)null;

	if(entity.GetType() == CGroup::GetClassName())
	{	
		if(Tight)
			u = new CLowspaceGroupUnit(this, dir, entity, type, MainView, MainVisualGraph, MainActiveGraph);
		else
			u = new CHighspaceGroupUnit(this, dir, entity, type, MainView, MainVisualGraph, MainActiveGraph); 
	}
	else
	{
		if(!GenerateAvatar(entity, type).IsEmpty())
		{
			u = new CSingleUnit(this, dir, entity, type, MainView, MainVisualGraph, MainActiveGraph);
		} 
		else
		{
			Log->ReportError(this, L"Can't open %s %s", type, entity.Object);
			return null;
		}
	}
	
	Units.push_back(u);
	return u;
}

void CWorldServer::Dealloc(CUnit * u)
{
	if(u->Lifespan == ELifespan::Permanent)
	{
		u->Save();
	}

	if(u->Parent)
	{
		u->Parent->Close(u);
	}

	Units.Remove(u);
	u->Free();
}

CUnit * CWorldServer::OpenEntity(CUol & entity, CString const & area, CShowParameters * f)
{
	auto u = FindUnit(entity);

	if(!u)
	{
		u = AllocateUnit(entity, Complexity);
	}

	if(u)
	{
		OpenUnit(u, area, f);
	}
	
	return u;
}

void CWorldServer::OpenUnit(CUnit * u, CString const & area, CShowParameters * f)
{
	///if(u->Avatarization->Model)
	///{
		Show(u, area, f);
	///}
	///else
	///{
	///	Dealloc(u);
	///	DeleteObject(u->Avatarization);
	///}
}

CUnit * CWorldServer::FindUnit(CUol & entity)
{
	auto load = [this](CUol & entity) -> CUnit *
				{
					CString e;
					auto l = MapUserGlobalPath(Name + L"/" + Layout + L".layout/" + entity.Object);
					
					if(Storage->Exists(l))
					{
						e = CPath::GetName(entity.Object);
					}

					if(e.empty())
					{
						for(auto & i : Storage->Enumerate(MapUserGlobalPath(Name + L"/" + Layout + L".layout"), L"Group-.*").Where([](auto & i){ return i.Type == CDirectory::GetClassName(); }))
						{
							auto l = CPath::Join(i.Path, CPath::GetName(entity.Object));

							if(Storage->Exists(l))
							{
								e = CPath::GetName(i.Path);
								break;
							}
						}
						
					}

					if(!e.empty())
					{
						CUnit * u = null;

						if(e.StartsWith(CGroup::GetClassName() + L"-"))
						{
							if(Tight)
								u = new CLowspaceGroupUnit(	this, 
															MapRelative(Name + L"/" + Layout + L".layout/" + e), 
															e,
															MainView,
															MainVisualGraph,
															MainActiveGraph);
							else
								u = new CHighspaceGroupUnit(this, 
															MapRelative(Name + L"/" + Layout + L".layout/" + e), 
															e,
															MainView,
															MainVisualGraph,
															MainActiveGraph);
						}
						else
						{
							u = new CSingleUnit(this, 
												MapRelative(Name + L"/" + Layout + L".layout/" + e), 
												MainView,
												MainVisualGraph,
												MainActiveGraph,
												e);
						}

						
						//u->Load(&CTonDocument(CXonTextReader(s)));
						Units.push_back(u);
	
					//	Storage->Close(s);
						return u;
					}

					return null;
				};

	auto u = Units.Find([&entity](CUnit * i){ return i->ContainsEntity(entity); });
	
	if(!u)
	{
		u = load(entity);
	}

	return u;
}

CUnit * CWorldServer::FindGroup(CArea * a)
{
	return null;
}

void CWorldServer::Show(CUnit * u, CString const & area, CShowParameters * f)
{
	auto prevMaster	= u->Parent;
	auto master		= (CArea *)null;
	auto origin		= CTransformation::Nan;

	if(area == AREA_LAST_INTERACTIVE)
	{
		master = Area->Find(u->LastInteractiveMaster);

		if(!master)
			master = Area->Match(u->GetDefaultInteractiveMasterTag());
	}
	else if(area == AREA_LAST)
	{
		master = Area->Find(u->LastMaster);

		if(!master)
			master = Area->Match(u->GetDefaultInteractiveMasterTag());
	}
	else
	{
		master = Area->Find(area); // master area by name

		if(!master)
			master = Area->Match(area); // master area by tag
	}

	auto hidings = CollectHidings(u, master);

	if(f && f->Pick.Active)
	{
		origin = (f->Pick.Active->FinalMatrix * f->Pick.Space->Matrix).Decompose();
		origin.Scale = {0.1f, 0.1f, 0.1f};
	}

	if(prevMaster != master)
	{
		if(prevMaster)
		{
			origin = u->Transformation;
			prevMaster->Remember(u);
			prevMaster->Close(u);
		}

		master->Open(u, EAddLocation::Known, MainViewport, f ? f->Pick : CPick(), origin);

		u->LastMaster = master->Name;

		if(master->Interactive)
			u->LastInteractiveMaster = master->Name;
		else
			u->LastNoninteractiveMaster = master->Name;
	}

	if(f)
	{
		if(f->Activate || prevMaster == master)
			master->Activate(u, null, f->Pick, origin);
	
		if(origin.IsReal() && f->Animation.IsReal())
		{
			auto t = u->Transformation;
			StartShowAnimation(u, f, origin, t);
		}
	}

	if((!prevMaster || !prevMaster->Interactive) && master->Interactive)
	{
		UnitOpened(u, u->Transformation, f);
	}

	Showings.push_back(u);

	for(auto i : hidings)
	{
		if(!Showings.Contains(i))
		{
			if(i->LastNoninteractiveMaster.empty())
			{
				Hide(i, null);
			}
			else
			{
				Show(i, i->LastNoninteractiveMaster, null);
			}
		}
	}
	
	Showings.Remove(u); // do after hiding!
	///Log->ReportMessage(this, L"Shown: %s  ->  %s  -> %s", u->Avatarization->Avatar->Url.Object, u->Name, master->Name);
}

void CWorldServer::StartShowAnimation(CArea * a, CShowParameters * f, CTransformation & from, CTransformation & to)
{
	a->Transform(from);
	RunAnimation(a, CAnimated<CTransformation>(from, to, f->Animation));
}

void CWorldServer::StartHideAnimation(CArea * u, CHideParameters * f, CTransformation & from, CTransformation & to, std::function<void()> hide)
{
	if(!f || !f->End.IsReal() || !Engine->IsRunning())
	{
		hide();
	} 
	else
	{
		auto ani = CAnimated<CTransformation>(from, f->End, Style->GetAnimation(L"Animation"));

		Core->AddJob(	this,
						L"Hide animation",
						[this, u, ani, hide]() mutable
						{	
							if(ani.Animation.Running)
							{
								u->Transform(ani.GetNext());
								Engine->Update();
								return false;
							} 
							else
							{
								hide();
								return true;
							}
						});
	}
}

void CWorldServer::Hide(CUnit * u, CHideParameters * p)
{
	if(!u->IsUnder(Area))
		return;

	if(Showings.Contains(u))
		return;

	if(Hidings.Contains(u))
		return;

	Hidings.push_back(u);

	u->Parent->Remember(u);

	auto hide = [this, u]
				{
					UnitClosed(u);

					auto master = u->Parent;

					master->Close(u);
					u->LastNoninteractiveMaster.clear();

					if(u->Lifespan == ELifespan::Visibility)
					{
						auto name = u->Name;
						Dealloc(u);
						master->Forget(name);
					}

					Hidings.Remove(u);
				};

	StartHideAnimation(u, p, u->Transformation, p->End, hide);
}

void CWorldServer::Attach(CElement * m, CUol & alloc)
{
	///auto a = Allocations.Find([m](auto i){ return i->Model == m; });
	///auto b = FindAllocation(alloc, CUol(), CUol());
	///
	///if(a && a->IsOpen())
	///{	
	///	a->Attached.push_back(alloc);
	///	b->Parents.push_back(a);
	///	auto b = OpenAllocation(alloc, AREA_LAST_INTERACTIVE, CShowFeatures());
	///	
	///	if(a->Avatar)
	///	{
	///		a->Avatar->GetInfo(Url)->Add(L"Attached")->Set(alloc);
	///	}
	///}
	throw CException(HERE, L"Not implemented");
}

void CWorldServer::Detach(CElement * m, CUol & l)
{
	///auto a = Allocations.Find([&](auto i){ return i->Model == m; });
	///auto b = FindAllocation(l, CUol(), CUol());
	///
	///if(a && a->IsOpen())
	///{	
	///	a->Attached.Remove(l);
	///	b->Parents.Remove(a);
	///	Hide(b, CHideFeatures());
	///
	///	if(a->Avatar)
	///	{
	///		auto p = a->Avatar->GetInfo(Url)->Find(L"Attached", l);
	///		a->Avatar->GetInfo(Url)->Remove(p);
	///	}
	///}
	throw CException(HERE, L"Not implemented");
}

bool CWorldServer::IsAttachedTo(CUol & l, CElement * to)
{
	///auto b = Allocations.Find([to](auto i){ return i->Avatar && i->Model == to; });
	///return b->Attached.Contains(l);
	throw CException(HERE, L"Not implemented");
}

bool CWorldServer::IsAttachedTo(CUol & l, CUol & to)
{
	///auto b = FindAllocation(to, CUol(), CUol());
	///return b->Attached.Contains(l);
	throw CException(HERE, L"Not implemented");
}
	
void CWorldServer::Drag(CArray<CDragItem> & d)
{
	for(auto i : d)
	{
		Drags.push_back(i);
	}
		
	DragAllocation = CreateAvatar(GenerateAvatar(CUol(d.front().Object), AVATAR_ICON2D), L"");
	///DragAllocation->Area->SetView(MainView);

	auto n = DragAllocation;
	n->Visual->Clipping = EClipping::No;
	n->Active->Clipping = EClipping::No;
	
	CAvatarMetrics m;
	m.FaceSize = CSize(48, 48, 48);
	m.FaceMargin = CFloat6(0);
	n->As<IFaceTitleAvatar>()->SetAppearance(ECardTitleMode::No, m);
	n->UpdateLayout(CLimits(m.FaceSize, m.FaceSize), true);
	

	throw CException(HERE, L"Need update");
	///MainVisualGraph->GetSpace(TopArea->Space)->AddNode(n->Visual);

	DragDefaultAvatar = DragAllocation;
	DragCurrentAvatar = DragAllocation;

	MainActiveGraph->Root->IsPropagator = false;
}

void CWorldServer::CancelDragDrop()
{
	if(DropTarget)
	{
		DropTarget->As<IDropTarget>()->Leave(Drags, DragCurrentAvatar);
		DropTarget->Free();
		DropTarget = null;
	}

	auto n = DragAllocation;
	
	throw CException(HERE, L"Need update");
	///MainVisualGraph->GetSpace(TopArea->Space)->RemoveNode(n->Visual);
	
	///Dealloc(DragAllocation);
	//DragAllocation->Free();
	
	Drags.clear();
	Engine->InputSystem->First<CMouse>()->SetImage(null);
	MainActiveGraph->Root->IsPropagator = true;
}

CUnit * CWorldServer::GetUnit(CActive * a)
{
	auto m = a->AncestorOwnerOf<CModel>();

	while(m && m->Parent && m->Parent->Active->AncestorOwnerOf<CModel>())
	{
		m = m->Parent->Active->AncestorOwnerOf<CModel>();
	}
	
	if(m)
	{
		return m->Unit;
	}
	else
	{
		return null;
	}
}

void CWorldServer::RunAnimation(CElement * n, CAnimated<CTransformation> a)
{
	n->Take();
	Core->AddJob(	this,
					L"Element animation",
					[this, n, a]() mutable 
					{	
						if(a.Animation.Running)
						{
							n->Transform(a.GetNext());
							Engine->Update();
							return false;
						} 
						else
						{
							n->Free();
							return true;
						}
					});
}

void CWorldServer::RunAnimation(CArea * n, CAnimated<CTransformation> a)
{
	n->Take();
	Core->AddJob(	this,
					L"Area animation",
					[this, n, a]() mutable 
					{	
						if(a.Animation.Running)
						{
							n->Transform(a.GetNext());
							Engine->Update();
							return false;
						} 
						else
						{
							n->Free();
							return true;
						}
					});
}

bool CWorldServer::CanExecute(const CUrq & u)
{
	if(CUol::IsValid(u))
	{
		CUol o(u);
		return o.GetType() == CGroup::GetClassName();
	}
	return false;
}

void CWorldServer::Execute(const CUrq & u, CExecutionParameters * p)
{
	OpenEntity(CUol(u), AREA_LAST_INTERACTIVE, dynamic_cast<CShowParameters *>(p));
}

CView * CWorldServer::Get(const CString & name)
{
	if(name == ThemeView->Name)
	{
		return ThemeView;
	}

	if(name == MainView->Name)
	{
		return MainView;
	}
	return null;
}

void CWorldServer::OnDiagnosticsUpdating(CDiagnosticUpdate & u)
{
	DiagGrid.Clear();

	std::function<void(CElement *, const CString &)> dumpn =	[this, &dumpn, &u](auto n, auto & s)
																{
																	if(Diagnostic->ShouldProceed(u, DiagGrid.GetSize()))
																	{
																		auto & r = DiagGrid.AddRow();
	
																		if(Diagnostic->ShouldFill(u, DiagGrid.GetSize()))
																		{
																			r.SetNext(s + n->Name);
																			r.SetNext(n->GetInstanceName());
																			r.SetNext(n->Transformation.Position.ToNiceString());
																			r.SetNext(n->Transformation.Rotation.ToNiceString());
																			r.SetNext(n->Transformation.Scale.ToNiceString());
																			r.SetNext(L"%7.0f %7.0f", n->Size.W, n->Size.H);
																			r.SetNext(L"%7.0f %7.0f", n->Slimits.Smax.W, n->Slimits.Smax.H);
																			r.SetNext(L"%7.0f %7.0f", n->Slimits.Pmax.W, n->Slimits.Pmax.H);
																			r.SetNext(n->GetStatus());
																		}
																			
																		for(auto i : n->Nodes)
																		{
																			dumpn(i, s + L"  ");
																		}
																	}
																};

	std::function<void(CPlacement *, const CString &)> dumpa =	[this, &dumpa, &dumpn, &u](auto a, auto & tab)
																{
																	if(!a->Area)
																	{
																		return;
																	}

																	if(Diagnostic->ShouldProceed(u, DiagGrid.GetSize()))
																	{
																		auto & r = DiagGrid.AddRow();
	
																		if(Diagnostic->ShouldFill(u, DiagGrid.GetSize()))
																		{
																			r.SetNext(tab + + L"~" + a->Name + L"~");
																			r.SetNext(a->Area->GetInstanceName());
																			r.SetNext(a->Area->Transformation.Position.ToNiceString());
																			r.SetNext(a->Area->Transformation.Rotation.ToNiceString());
																			r.SetNext(a->Area->Transformation.Scale.ToNiceString());
																			r.SetNext(L"");
																			r.SetNext(L"");
																			r.SetNext(L"");
																			r.SetNext(CString::Join(a->Area->Tags, L" "));
																		}

																		if(auto g = a->Area->As<CGroupUnit>())
																		{
																			for(auto i : g->Header->Tabs)
																			{
																				if(i->Model)
																					dumpn(i->Model, tab + L"  ");
																			}
																		}
																		else if(auto u = a->Area->As<CSingleUnit>())
																		{
																			dumpn(u->Model, tab + L"  ");
																		}

																		for(auto i : a->Area->Areas)
																		{
																			dumpa(i, tab + L"  ");
																		}
																	}
																};
	for(auto i : Area->Areas)
	{
		dumpa(i, L"");
	}

	Diagnostic->Add(u, DiagGrid);
}

CProtocolConnection<IAvatarProtocol> CWorldServer::FindAvatarSystem(CUol & e, CString const & type)
{
	CList<CUol> avs;

	auto avatarProtocol = Nexus->Connect<IAvatarProtocol>(this, e, AVATAR_PROTOCOL);

	if(avatarProtocol)
	{
		avs = avatarProtocol->GenerateSupportedAvatars(e, type);
		if(!avs.empty())
		{
			return avatarProtocol;
		}
	}
	else
	{
		for(auto & i : Nexus->ConnectMany<IAvatarProtocol>(this, AVATAR_PROTOCOL))
		{
			avs = i->GenerateSupportedAvatars(e, type);
			if(!avs.empty())
			{
				return i;
			}
		}
	}

	return CProtocolConnection<IAvatarProtocol>();
}

CElement * CWorldServer::CreateElement(CString const & name, CString const & type)
{
	//if(type == CWindow::GetClassName())		return new CWindow(this, Hub, Style, name); else
	if(type == CText::GetClassName())		return new CText(this, Style, name); else
	if(type == CTextEdit::GetClassName())	return new CTextEdit(this, Style, name); else
	if(type == CButton::GetClassName())		return new CButton(this, Style, name); else
	if(type == CStack::GetClassName())		return new CStack(this, Style, name); else
	if(type == CTable::GetClassName())		return new CTable(this, Style, name); else

	return null;
}


CNexusObject * CWorldServer::GetEntity(CUol & e)
{
	return Server->FindObject(e);
}

CList<CUol> CWorldServer::GenerateSupportedAvatars(CUol & e, CString const & type)
{
	CList<CUol> l;

	auto p = CMap<CString, CString>{{L"entity", e.ToString()}, {L"type", type}};

	if(e.GetType() == CGroup::GetClassName())
	{
		if(type == AVATAR_ICON2D)	l.push_back(CUol(Url, CGuid::Generate64(CGroupIcon::GetClassName()), p));
	}

	return l;

}

CAvatar * CWorldServer::CreateAvatar(CUol & u)
{
	CAvatar * a = null;
	
	if(u.Server == Url.Server)
	{
		if(u.GetType() == CGroupIcon::GetClassName())	a = new CGroupIcon(this, u.Object); else

		if(u.GetType() == CDefaultIcon::GetClassName())	a = new CDefaultIcon(this, u.Object); else

		return null;
	}

	a->Url = u;
	RegisterObject(a, false);
	a->Free();
	
	return a;
}

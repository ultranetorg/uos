#pragma once
#include "WorldServer.h"

namespace uos
{
	class CMobileWorld : public CWorldServer
	{
		public:
			CScreenViewport *			SkinViewport = null;
			CMobileSkinModel *			Skin = null;
			CWorldView *				SkinView = null;

			CPolygonalPositioning *		Positioning;	

			CMobileWorld(CLevel2 * l, CServerInfo * si) : CWorldServer(l, si) 
			{
				Name		= WORLD_MOBILE_EMULATION;
				Complexity	= AVATAR_WIDGET;
				Free3D		= false;
				FullScreen	= true;
				Tight		= true;
			}

			~CMobileWorld()
			{
				Skin->Free();
				Positioning->Free();
				delete SkinView;
			}

			void InitializeViewports() override
			{
				auto s = Engine->ScreenEngine->PrimaryScreen;
				auto t = Targets.Find([s](auto i){ return i->Screen = s; });

				MainViewport	= new CScreenViewport(&Engine->EngineLevel, t, 17 * t->Device->Display->Scaling.x, 84 * t->Device->Display->Scaling.y, 370, 628, 370 * t->Device->Display->Scaling.x, 628 * t->Device->Display->Scaling.y);
				SkinViewport	= new CScreenViewport(&Engine->EngineLevel, t, 0, 0, t->Size.W / t->Device->Display->Scaling.x, t->Size.H / t->Device->Display->Scaling.y, t->Size.W, t->Size.H);

				SkinViewport->Tags = {L"Skin"};
				MainViewport->Tags = {L"Apps", AREA_SERVICE_BACK, AREA_SERVICE_FRONT};

				Viewports.push_back(MainViewport);
				Viewports.push_back(SkinViewport);
			}
			
			void InitializeView() override
			{
				Z = 1000;

				auto ma = MainViewport->W/MainViewport->H;
				auto fovm = 2.f * ma * atan((MainViewport->W * 0.5f)/(ma * Z));

				auto sa = SkinViewport->W/SkinViewport->H;
				auto fovs = 2.f * sa * atan((SkinViewport->W * 0.5f)/(sa * Z));

				MainView	= new CWorldView(this, Engine, L"Main");
				HudView		= new CWorldView(this, Engine, L"Hud");
				ThemeView	= new CWorldView(this, Engine, L"Theme");
				SkinView	= new CWorldView(this, Engine, L"Skin");

				HudView->	AddCamera(MainViewport, fovm, 100, 1e4)->UseAffine();
				MainView->	AddCamera(MainViewport, fovm, 100, 15000)->UseAffine();
				ThemeView->	AddCamera(MainViewport, fovm, 10, 1e6)->UseAffine();
				SkinView->	AddCamera(SkinViewport, fovs, 10, 10000)->UseAffine();
			}

			void InitializeGraphs() override
			{
				MainActiveGraph->Root->KeyboardInput += ThisHandler(OnKeyboard);
			}

			void InitializeAreas() override
			{
				
				auto w = MainViewport->W;
				auto h = MainViewport->H;

				Positioning = new CPolygonalPositioning();

				Positioning->GetView				= [this]			{ return MainView; };
				Positioning->Transformation[null]	= [this, w, h](auto){ return CTransformation(0, 0, Z); };
				Positioning->Bounds[null]			= [w, h]			{ return CArray<CFloat2>{{-w/2, -h/2}, {-w/2, h/2}, {w/2, h/2}, {w/2, -h/2}}; };

				FieldArea->	SetPositioning(Positioning);
				MainArea->	SetPositioning(Positioning);
				HudArea->	SetPositioning(Positioning);
				TopArea->	SetPositioning(Positioning);

				ServiceBackArea->As<CPositioningArea>()->SetPositioning(Positioning);
				ServiceFrontArea->As<CPositioningArea>()->SetPositioning(Positioning);
				
				Area->Match(AREA_SKIN)->As<CPositioningArea>()->SetPositioning(Positioning);
				Area->Match(AREA_SKIN)->As<CPositioningArea>()->SetView(SkinView);

				for(auto i : Area->Areas)
				{
					if(auto p = i->Area->As<CPositioningArea>())
					{
						p->PlaceNewDefault			= Positioning->PlaceCenter; 
						p->PlaceNewConvenient		= Positioning->PlaceCenter;
						p->PlaceNewExact			= Positioning->PlaceCenter;
					}
				}

			}

			void InitializeModels() override
			{
				Skin = new CMobileSkinModel(this);

				auto a = AllocateUnit(Skin);
				Show(a, AREA_SKIN, null);
			}

			CList<CUnit *> CollectHidings(CArea * a, CArea * master) override
			{
				CList<CUnit *> hidings;

				bool hidefs = false; 
				bool hidems = false; 
				bool hidehs = false; 
				
				if(master == FieldArea)
				{
					hidefs = hidems = hidehs = true;
				}
				if(master == MainArea)
				{
					hidems = hidehs = true;
				}
				if(master == HudArea)
				{
					hidehs = true;
				}
				
				for(auto b : Units.Where([&](CUnit * i){ return	(	(i != a) &&
																	(hidefs && i->Parent && FieldArea->ContainsDescedant(i) || 
																	hidems && i->Parent && MainArea->ContainsDescedant(i) || 
																	hidehs && i->Parent && HudArea->ContainsDescedant(i))); }))
				{
					hidings.push_back(b);
				}

				return hidings;
			}

			void OnKeyboard(CActive * r, CActive * s, CKeyboardArgs * arg)
			{
				if(arg->Class == EInputClass::Keyboard && arg->Action == EInputAction::On)
				{
					if(GlobalHotKeys.Contains(arg->Control))
					{
						GlobalHotKeys[arg->Control](arg->Control);
						arg->StopPropagation = true;
					}
				}
			}

			void StartShowAnimation(CArea * a, CShowParameters * f, CTransformation & from, CTransformation & to) override
			{
				auto newfrom = CTransformation::Nan;

				if(auto arg = dynamic_cast<CTouchArgs *>(f->Args))
				{
					auto d = arg->Input->Touch->Position - arg->Input->Touch->Origin;

					if(d.x > 0)	newfrom = CTransformation(-MainViewport->W * 1.5f, -MainViewport->H * 0.5f, to.Position.z); else
					if(d.x < 0)	newfrom = CTransformation( MainViewport->W * 1.5f, -MainViewport->H * 0.5f, to.Position.z); else
					if(d.y > 0)	newfrom = CTransformation(-MainViewport->W * 0.5f, -MainViewport->H * 1.5f, to.Position.z); else
					if(d.y < 0)	newfrom = CTransformation(-MainViewport->W * 0.5f,  MainViewport->H * 1.5f, to.Position.z);
				}

				if(newfrom.IsReal())
				{
					a->Transform(newfrom);
					RunAnimation(a, CAnimated<CTransformation>(newfrom, to, f->Animation));
				}
				else
					__super::StartShowAnimation(a, f, from, to);
			}

			void StartHideAnimation(CArea * a, CHideParameters * f, CTransformation & from, CTransformation & to, std::function<void()> hide) override
			{
				auto newto = CTransformation::Nan;

				if(f && f->Args)
				{
					if(auto arg = f->Args->As<CTouchArgs>())
					{
						auto d = arg->Input->Touch->Position - arg->Input->Touch->Origin;
	
						if(d.x < 0)	newto = CTransformation(-MainViewport->W * 1.5f, -MainViewport->H * 0.5f, from.Position.z); else 
						if(d.x > 0)	newto = CTransformation( MainViewport->W * 1.5f, -MainViewport->H * 0.5f, from.Position.z); else 
						if(d.y > 0)	newto = CTransformation(-MainViewport->W * 0.5f,  MainViewport->H * 1.5f, from.Position.z); else 
						if(d.y < 0)	newto = CTransformation(-MainViewport->W * 0.5f, -MainViewport->H * 1.5f, from.Position.z);

						auto ani = CAnimated<CTransformation>(from, newto, f->Animation);

						Core->AddJob(	this,
										L"Hide animation",
										[this, a, ani, hide]() mutable
										{	
											if(ani.Animation.Running)
											{
												a->Transform(ani.GetNext());
												Engine->Update();
												return false;
											} 
											else
											{
												hide();
												return true;
											}
										});
						return;
					}
				}

				__super::StartHideAnimation(a, f, from, to, hide);
			}
	};
}


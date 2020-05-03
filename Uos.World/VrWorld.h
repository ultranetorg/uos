#pragma once
#include "DesktopWorld.h"

namespace uos
{
	class CVrWorld : public CDesktopWorld
	{
		public:
			CPolygonalPositioning *						FrontPositioning;
			CPolygonalPositioning *						NearPositioning;
			CCylindricalPositioning *					InteractivePositioning;
			CCylindricalPositioning *					BackPositioning;

			CPositioningArea *							NearArea;

			CAnimated<CFloat3>							MainAnimatedDirection;
			CAnimated<CFloat3>							NearAnimatedDirection;

			CVrWorld(CLevel2 * l, CServerInfo * si) : CDesktopWorld(l, si) 
			{
				Name		= WORLD_VR_EMULATION;
				Complexity	= AVATAR_ENVIRONMENT;
				Free3D		= true;
				FullScreen	= false;
				Tight		= false;

				UnitOpened +=	[this](auto u, auto t, auto f)
								{
									if(u->IsUnder(MainArea) || u->IsUnder(FieldArea))
									{
										if(u->AncestorOf<CPositioningArea>()->Positioning == InteractivePositioning)
										{
											auto d = MainView->PrimaryCamera->Direction;
											d.y = 0;

											DirectCameras(d);
										}
									}
								};
			}

			~CVrWorld()
			{
				FrontPositioning->Free();
				NearPositioning->Free();
				InteractivePositioning->Free();
				BackPositioning->Free();
			}

			void InitializeGraphs() override
			{
				__super::InitializeGraphs();

//				MainActiveGraph->Root->MoveInput += ThisHandler(OnMoveInput);
			}

			void InitializeView() override
			{
				__super::InitializeView();

				NearView = new CWorldView(this, Engine, L"Near");
				NearView->AddCamera(MainViewport, CAngle::ToRadian(90.f), 500.f, 10000);

				HudView->PrimaryCamera->UseAffine();
				MainView->PrimaryCamera->UseLookToLeftHand();
				NearView->PrimaryCamera->UseLookToLeftHand();
				ThemeView->PrimaryCamera->UseLookToLeftHand();
			}

			void InitializeAreas() override
			{
				auto w = MainViewport->W;
				auto h = MainViewport->H;

				NearArea = Area->Match(AREA_NEAR)->As<CPositioningArea>();

				FrontPositioning		= new CPolygonalPositioning();
				NearPositioning			= new CPolygonalPositioning();
				InteractivePositioning	= new CCylindricalPositioning(this, h, Z);
				BackPositioning			= new CCylindricalPositioning(this, h, Z * 6.f);

				FrontPositioning->GetView					= [this]		{ return HudView; };
				FrontPositioning->Transformation[null]	= [this](auto)	{ return CTransformation(0, 0, Z); };
				FrontPositioning->Bounds[null]			= []			{ return CArray<CFloat2>{{-FLT_MAX, -FLT_MAX}, {-FLT_MAX, FLT_MAX}, {FLT_MAX, FLT_MAX}, {FLT_MAX, -FLT_MAX}}; };

				NearPositioning->GetView				= [this]		{ return NearView; };
				NearPositioning->Transformation[null]	= [this](auto)	{ return CMatrix::FromTransformation({0, 0, Z-200}, {CFloat::PI/2, 0, 0}, {0, 0, -Z}, {1, 1, 1}).Decompose(); };
				NearPositioning->Bounds[null]			= [w]			{ return CArray<CFloat2>{{-w, 1000}, {-w, 10000}, {w, 10000}, {w, 1000}}; };
				
				InteractivePositioning->GetView				= [this]	{ return MainView; };
				InteractivePositioning->Matrixes[null]			= [](auto)	{ return CTransformation::Identity; };
	
				BackPositioning->GetView				= [this]	{ return MainView; };
				BackPositioning->Matrixes[null]			= [](auto)	{ return CTransformation::Identity; };

				MainArea->MaxSize =	 [](auto vp){ return CSize(vp->W - 200, vp->H - 200, FLT_MAX); };
				FieldArea->MaxSize = [](auto vp){ return CSize(vp->W - 200, vp->H - 200, FLT_MAX); };

				TopArea			->SetPositioning(FrontPositioning);
				HudArea			->SetPositioning(FrontPositioning);
				NearArea		->SetPositioning(NearPositioning);
				MainArea		->SetPositioning(InteractivePositioning, true);
				FieldArea		->SetPositioning(InteractivePositioning, true);
				BackArea		->SetPositioning(BackPositioning, false);

				TopArea->PlaceNewDefault		= HudArea->PlaceNewDefault		= FrontPositioning->PlaceCenter; 
				TopArea->PlaceNewConvenient		= HudArea->PlaceNewConvenient	= FrontPositioning->PlaceCenter;
				TopArea->PlaceNewExact			= HudArea->PlaceNewExact		= FrontPositioning->PlaceCenter;		
				
				MainArea->PlaceNewDefault		= FieldArea->PlaceNewDefault	= InteractivePositioning->PlaceCenter; 
				MainArea->PlaceNewConvenient	= FieldArea->PlaceNewConvenient	= InteractivePositioning->PlaceCenter;
				MainArea->PlaceNewExact			= FieldArea->PlaceNewExact		= InteractivePositioning->PlaceCenter;		
				
				BackArea->PlaceNewDefault		= BackPositioning->PlaceExact; 
				BackArea->PlaceNewConvenient	= BackPositioning->PlaceExact;
				BackArea->PlaceNewExact			= BackPositioning->PlaceExact;		
					
				NearArea->PlaceNewDefault =	[](auto c, auto pp, auto & pk, auto & s)
											{
												return CTransformation(-s.W/2, 2000 + -s.H/2, 0);
											};
			}

			void OnMouse(CActive * r, CActive * s, CMouseArgs * arg) override
			{
				if(	arg->Class == EInputClass::Mouse &&
					arg->Capture.IsCaptured() && 
					arg->Capture.Pick.Active == Sphere->Active &&
					arg->Capture.Message.Control == EControl::MiddleButton && (arg->Event == EGraphEvent::Hover || arg->Event == EGraphEvent::Roaming))
				{
					auto w = MainView->PrimaryCamera->Viewport->W;
					auto h = MainView->PrimaryCamera->Viewport->H;
		
					auto d = arg->Input->PositionDelta;
					auto dv = MainView->PrimaryCamera->Raycast(CFloat2(w/2, h/2) + d).Direction;
					auto cv = MainView->PrimaryCamera->Raycast(CFloat2(w/2, h/2)).Direction;
					MainView->PrimaryCamera->SetDirection((MainView->PrimaryCamera->Direction + (dv - cv)).GetNormalized());

					dv = ThemeView->PrimaryCamera->Raycast(CFloat2(w/2, h/2) + d).Direction;
					cv = ThemeView->PrimaryCamera->Raycast(CFloat2(w/2, h/2)).Direction;
					ThemeView->PrimaryCamera->SetDirection((ThemeView->PrimaryCamera->Direction + (dv - cv)).GetNormalized());
		
					//d.x = 0;
					dv = NearView->PrimaryCamera->Raycast(CFloat2(w/2, h/2) + d).Direction;
					cv = NearView->PrimaryCamera->Raycast(CFloat2(w/2, h/2)).Direction;
					auto cc = (dv - cv);
					cc.x = cc.z = 0;
					NearView->PrimaryCamera->SetDirection((NearView->PrimaryCamera->Direction + cc).GetNormalized());
				}
				else 
					__super::OnMouse(r, s, arg);
			}

			void DirectCameras(CFloat3 & d)
			{
				MainAnimatedDirection = CAnimated<CFloat3>(MainView->PrimaryCamera->Direction, d, Style->GetAnimation(L"Animation"));
				Core->AddJob(this, L"Main camera directing",	[this]
																{ 
																	if(MainAnimatedDirection.Animation.Running)
																	{
																		auto d = MainAnimatedDirection.GetNext();
																		MainView->PrimaryCamera->SetDirection(d);
																		ThemeView->PrimaryCamera->SetDirection(d);
																		Engine->Update();
																	}
																	return !MainAnimatedDirection.Animation.Running;
																});

				auto delta = (d - NearView->PrimaryCamera->Direction);
				delta.x = delta.z = 0;

				NearAnimatedDirection = CAnimated<CFloat3>(NearView->PrimaryCamera->Direction, (NearView->PrimaryCamera->Direction + delta).GetNormalized(), Style->GetAnimation(L"Animation"));
				Core->AddJob(this, L"Near camera directing",	[this]
																{ 
																	if(NearAnimatedDirection.Animation.Running)
																	{
																		auto d = NearAnimatedDirection.GetNext();
																		NearView->PrimaryCamera->SetDirection(d);
																		Engine->Update();
																	}
																	return !NearAnimatedDirection.Animation.Running;
																});
			}

	};
}

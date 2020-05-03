#pragma once
#include "Text.h"

namespace uos
{
	class CMobileSkinModel : public CModel
	{
		public:
			CWorld *									Level;
			
			CText *										SwipeLeft;
			CText *										SwipeRight;
			CText *										SwipeDown;
			CText *										SwipeUp;
			
			CSolidRectangleMesh *						Frame;

			CList<CTouch *> Touches;

			UOS_RTTI
			CMobileSkinModel(CWorld * l) : CModel(l, l->Server, ELifespan::Visibility, GetClassName())
			{
				Level = l;
				Tags = {L"Skin"};

				auto path = Level->Server->MapPath(L"Phone.png");
				auto f = Level->Storage->OpenReadStream(path);
				auto t = Level->Engine->TextureFactory->CreateTexture();
				t->Load(f);
				Level->Storage->Close(f);

				auto m = new CMaterial(&Level->Engine->EngineLevel, Level->Engine->PipelineFactory->DiffuseTextureShader);
				m->AlphaBlending = true;
				m->Textures[L"DiffuseTexture"] = t;
				m->Samplers[L"DiffuseSampler"].SetFilter(ETextureFilter::Point, ETextureFilter::Point, ETextureFilter::Point);
				Visual->SetMaterial(m);

				Frame = new CSolidRectangleMesh(&l->Engine->EngineLevel);
				Visual->SetMesh(Frame);
				Active->SetMesh(Frame);
				Frame->Free();

				m->Free();
				t->Free();

				Active->MouseInput += ThisHandler(OnFrameButtonEvent);
				
				SwipeLeft	= new CText(l, l->Style, L"SwipeLeft",	true);
				SwipeRight	= new CText(l, l->Style, L"SwipeRight", true);
				SwipeDown	= new CText(l, l->Style, L"SwipeDown",	true);
				SwipeUp		= new CText(l, l->Style, L"SwipeUp",	true);

				SwipeLeft	->SetText(L"Click to Swipe Left");
				SwipeRight	->SetText(L"Click to Swipe Right");
				SwipeDown	->SetText(L"Click to Swipe Down");
				SwipeUp		->SetText(L"Click to Swipe Up");

				Touches.push_back(new CTouch());

				auto w = 370.f-1; 
				auto h = 628.f-1;

				auto swipe =	[this](CMouseArgs * arg, float x, float y, float dx, float dy) mutable
								{
									auto move = [this, arg](CTouch * t, float x, float y, float dx, float dy, ETouchAction a)
												{
													CInputMessage m;
													m.Class		= EInputClass::TouchScreen;
													m.Action	= EInputAction::Move;
													m.Control	= EControl::Screen;
													m.Screen	= arg->Screen;
													m.Id		= Level->Engine->InputSystem->GetNextID();
													
													auto v = m.MakeValues<CTouchInput>();
													
													v->Touch	= t;
													v->Touches	= &Touches;
													v->Action	= a;

													t->Primary = true;
													t->Origin = {x, y};
													t->Position.x = t->Origin.x + dx;
													t->Position.y = t->Origin.y + dy;

													Level->Engine->InputSystem->SendInput(m);
												};
									
									move(*Touches.begin(), x, y, 0, 0, ETouchAction::Added);
									move(*Touches.begin(), x, y, dx/2, dy/2, ETouchAction::Movement);
									move(*Touches.begin(), x, y, dx, dy, ETouchAction::Removed);

								};


				SwipeRight->Active->MouseInput +=	[this, swipe, w, h](auto, auto, auto a) mutable
													{
														if(a->Class == EInputClass::Mouse && a->Event == EGraphEvent::Click)
														{
															swipe(a, 0, h/2, 100, 0);
														}
													};

				SwipeLeft->Active->MouseInput +=	[this, swipe, w, h](auto, auto, auto a) mutable
													{
														if(a->Class == EInputClass::Mouse && a->Event == EGraphEvent::Click)
														{
															swipe(a, w, h/2,  -100, 0);
														}
													};

				SwipeUp->Active->MouseInput +=		[this, swipe, w, h](auto, auto, auto a) mutable
													{
														if(a->Class == EInputClass::Mouse && a->Event == EGraphEvent::Click)
														{
															swipe(a, w/2, 0, 0, 100);
														}
													};

				SwipeDown->Active->MouseInput +=	[this, swipe, w, h](auto, auto, auto a) mutable
													{
														if(a->Class == EInputClass::Mouse && a->Event == EGraphEvent::Click)
														{
															swipe(a, w/2, h, 0, -100);
														}
													};

				AddNode(SwipeLeft);
				AddNode(SwipeRight);
				AddNode(SwipeDown);
				AddNode(SwipeUp);
			}

			~CMobileSkinModel()
			{
				for(auto i : Touches)
				{
					delete i;
				}

				RemoveNode(SwipeLeft);
				RemoveNode(SwipeRight);
				RemoveNode(SwipeDown);
				RemoveNode(SwipeUp);

				SwipeLeft->Free();
				SwipeRight->Free();
				SwipeDown->Free();
				SwipeUp->Free();
			}

			virtual void DetermineSize(CSize & smax, CSize & s) override
			{
				Express(L"W", [smax]{ return smax.W; });
				Express(L"H", [smax]{ return smax.H; });
				UpdateLayout(CLimits(smax, smax), false);
			}

			virtual CTransformation DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t) override
			{
				return t;
			}

			void virtual UpdateLayout(CLimits const & l, bool apply) override
			{
				__super::UpdateLayout(l, apply);

				Frame->Generate(0, 0, Size.W, Size.H);

				SwipeRight->Transform(CTransformation(CFloat3(1, (Size.H + SwipeLeft->W)/2.f, Z_STEP), CQuaternion(0, 0, float(-M_PI_2)), 1.f));
				SwipeLeft->Transform(CTransformation(CFloat3(Size.W - SwipeRight->H, (Size.H + SwipeRight->W)/2.f, Z_STEP), CQuaternion(0.f, 0.f, -float(M_PI_2)), 1.f));
				SwipeDown->Transform(ceil((Size.W - SwipeDown->W)/2.f), Size.H - SwipeDown->H - 64, Z_STEP);
				SwipeUp->Transform(ceil((Size.W - SwipeUp->W)/2.f), 66, Z_STEP);
			}

			void OnFrameButtonEvent(CActive *, CActive *, CMouseArgs * a)
			{
				if(a->Class == EInputClass::Mouse && a->Event == EGraphEvent::Click)
				{
					if(CRect(169, 17, 106, 45).Contains(a->Pick.Point.ToXY()))
					{
						CInputMessage im;

						im.Class	= EInputClass::Keyboard;
						im.Control	= EControl::MobileHome;
						im.Action	= EInputAction::On;
						im.MakeValues<CKeyboardInput>();

						Level->Engine->InputSystem->SendInput(im);

						im.Action	= EInputAction::Off;
						//im.MakeValues<CKeyboardInput>();
						Level->Engine->InputSystem->SendInput(im);
					}
				}
			}
	};
}

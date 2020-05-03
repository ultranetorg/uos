#pragma once
#include "Element.h"
#include "Positioning.h"

namespace uos
{
	struct CResizing
	{
		//CFloat3		Delta;
		CFloat3		PositionDelta;
		CSize		Size;
	};

	class UOS_WORLD_LINKING CRectangleSizer
	{
		public:
			CWorldLevel *								Level;
			CElement *								Gripper = null;
			CElement *								Resizable = null;
			CView *										View;
			//CFloat3									CapturePosition;
			CMatrix										CaptureMatrix;// = Active->FinalMatrix;
			CFloat3										CapturePoint;// = CPlane(CFloat3(0), CFloat3(0, 0, -1)).GetRayIntersection(Active->GetActualView()->GetProjector(a.Movement.Viewport)->GetScanRay(a.Movement.Position).Transform(CaptureMatrix.GetInversed())); //  ray in This
			CSize										CaptureSize;
			CSize										CaptureSizeLast;
			//float										LastW;
			//float										LastH;
			bool										InSizing = false;
			bool										KeepAspect = false;
			CSize										Min = CSize(0, 0, 0);
			CPositioning *								Positioning;
			CPositioningCapture							PositioningCapture;
				
			Delegate<bool(CPick &)>						InGripper;
			Delegate<CPositioning * (CPick &)>			Captured;
			Delegate<void(CResizing &)>					Resizing;
			Delegate<void(CResizing &)>					Adjusting;
			Delegate<void()>							Resized;

			CRectangleSizer(CWorldLevel * l)
			{
				Level = l;
			}

			~CRectangleSizer()
			{
				if(Gripper)
				{
					Gripper->Active->MouseInput			-= ThisHandler(OnMouse);
					Gripper->Free();
				}
			}

			void SetGripper(CElement * n)
			{
				Gripper = n;

				Gripper->Active->MouseInput			+= ThisHandler(OnMouse);
			
				Gripper->Take();
			}

			void SetTarget(CElement * n)
			{
				Resizable = n;
					
					///InGripArea =[this](auto & is)
					///			{
					///				return InRightBottomCorner(Node, is);
					///			};
										
					///Resizing =	[this](auto & r)
					///			{
					///				Node->Express(L"IW", [r](){ return r.Size.W; });
					///				Node->Express(L"IH", [r](){ return r.Size.H; });
					///
					///				Node->UpdateLayout();
					///				Node->Transform(Node->Transformation * CTransformation(r.PositionDelta));
					///			};
				
			}

			///void SetSize(CSize & a)
			///{
			///	Node->Express(L"IW", [a](){ return a.W; });
			///	Node->Express(L"IH", [a](){ return a.H; });
			///}
			
			CPositioningCapture Capture(CViewport * vp, CFloat2 & vpp, CSize & size)
			{
				CPositioningCapture c;
			
				c.Size = size;
			
				auto v = View;
				auto m = CaptureMatrix;
			
				auto r = v->GetCamera(vp)->Raycast(vpp).Transform(!m); // ortho ray (in Surface CS)
				c.Point = CPlane(0, 0, -1).Intersect(r); // intersection with Field XY plane (in Surface CS)
				c.Offset = {0, 0, 0};
			
				return c;
			}

			void CaptureXY(CPick & p)
			{
				View			= p.Space->GetActualView();
				CaptureMatrix	= Resizable->Active->FinalMatrix * p.Space->Matrix;
				CapturePoint	= CPlane(0, 0, -1).Intersect(View->GetCamera(p.Camera->Viewport)->Raycast(p.Vpp).Transform(!CaptureMatrix)); //  ray in This
				CaptureSize		= CaptureSizeLast = Resizable->Size;
	
				PositioningCapture = Capture(p.Camera->Viewport, p.Vpp, CSize::Empty);
				///Capture = Area->Capture(Node->Active, Node->Size, p);
			}
			
			CResizing ResizeXYRB(CPick & pick)
			{
				CFloat2 _vpp;

				if(Positioning)
				{
					 auto p = Positioning->Move(PositioningCapture, pick).Position; // new pos in Grid
					 _vpp = Positioning->Project(pick.Camera->Viewport, pick.Space, p).ToXY();// View->GetProjector(vp)->ProjectVertex(p.Transform(Mover->Matrix)); // new pos on screen
				} 
				else
				{
					_vpp = pick.Vpp;
				}


				auto r = View->GetCamera(pick.Camera->Viewport)->Raycast(_vpp).Transform(!CaptureMatrix); //scan ray of new pos in This
				auto p = CPlane(0, 0, -1).Intersect(r); // new pos in This
				p.z = 0;

				auto d = CapturePoint - p;
				
				auto w = CaptureSize.W - d.x;
				auto h = CaptureSize.H + d.y;

				w = max(24, w);
				h = max(24, h);
				
				if(KeepAspect)
				{
					auto a = CaptureSize.W/CaptureSize.H;
					if(w/h != a)
					{
						if(w/CaptureSize.W < h/CaptureSize.H)
						{
							h = w/a;
						}
						else if(w/CaptureSize.W > h/CaptureSize.H)
						{
							w = h * a;
						}
					}
				}

				CResizing o;
				
				o.Size.W = max(w, Min.W);
				o.Size.H = max(h, Min.H);
				o.Size.D = 0;

				Resizing(o);

				o.PositionDelta.x = 0;
				o.PositionDelta.y = CaptureSizeLast.H - Resizable->Size.H; ///o.Size.H;;
				o.PositionDelta.z = 0;

				CaptureSizeLast = Resizable->Size;

				Adjusting(o);

				return o;
			}

			void OnMouse(CActive * r, CActive * s, CMouseArgs * a)
			{
				if(Gripper)
				{
					if(s == Gripper->Active)
					{
						auto p = a->Pick.Point;
				
						if(a->Control == EControl::LeftButton && a->Action == EInputAction::On && InGripper(a->Pick))
						{
							Positioning = Captured(a->Pick);

							InSizing = true;
							CaptureXY(a->Pick);
							a->StopPropagation = true;
						}
					}

					if(a->Event == EGraphEvent::Feedback)
					{
						auto d = a->Device->As<CMouse>();

						if(s == Gripper->Active)
						{
							if(InGripper(a->Pick))
							{
								d->SetImage(LoadCursor(null, IDC_SIZENWSE));
							}
						}
	
						if(InSizing)
						{
							d->SetImage(LoadCursor(null, IDC_SIZENWSE));
						}
					}

					if(InSizing)
					{
						if(a->Pick.Active && (a->Event == EGraphEvent::Hover || a->Event == EGraphEvent::Roaming))
						{
							auto r = ResizeXYRB(a->Pick);
							
							a->StopPropagation = true;
							//!!! Metrics.FaceArea = [r]{ return r.Area; };
	
						}
	
						if(a->Event == EGraphEvent::Released)
						{
							InSizing = false;
							if(Resized)
								Resized();

							a->StopPropagation = true;
							a->StopRelatedPropagation = true;
						}
					}
				}
			}

			bool InRightBottomCorner(CElement * e, CPick & is)
			{
				auto p = e->Active->Transit(is.Active, is.Point);
				auto mbp = e->InMargin(p) || e->InBorder(p) || e->InPadding(p);
				return mbp && (e->W - p.x <= e->M.RT + e->B.RT + e->P.RT) && (p.y <= e->M.BM + e->B.BM + e->P.BM);
			}
	};
}

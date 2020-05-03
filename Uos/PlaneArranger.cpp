#include "StdAfx.h"
#include "PlaneArranger.h"

using namespace uos;

CPlaneArranger::CPlaneArranger()
{
	//RootParameter = Config->GetParameter(L"Size");
	ApplyParameters();
}

CPlaneArranger::~CPlaneArranger()
{
}

void CPlaneArranger::ApplyParameters()
{
	//NormalDistance	= RootParameter->GetFloat32(L"DefaultDistance");
	Plane			= CPlane(CFloat3(0, 0, NormalDistance), CFloat3(0, 0, -1));
}

/*
void CPlaneArranger::ProcessNodeMouseButtonEvent(IArrangable * wn, CCursorEventArgs & a)
{
	if(a.InputMessage->Action == EInputAction::On)
	{
		CPlaneAdapter * ad = (CPlaneAdapter *)wn->GetAdapter(this);

		if(a.InputMessage->Sender == 1) 
		{
			CRay r = a.Intersection->GetRay();
			
			CapturedX	= ad->X - sin(r.Direction.GetAngleToYZ()) * a.Intersection->GetDistance();
			//CapturedX	= ad->X	- tan(r.Direction.GetAngleToYZ()) * ad->Distance;
			CapturedY	= ad->Y - sin(r.Direction.GetAngleToXZ()) * a.Intersection->GetDistance();
			//CapturedY	= ad->Y	- tan(r.Direction.GetAngleToXZ()) * ad->Distance;
		}
	}		
}

/// ???????? ???????? ????????? ? ?? ?????????, ?.?. ? ????? ??????????? ???? ? ?????????,
/// ??????? ?? ????? ?????? ? ????? ?????? ??????? ????????????? ????????
/// ? ?????? ????? ????????????? ? ????? ??????????? ??????? ? ?????, 
/// ?? ??? ??? ???????? is->GetDistance() ????? ?????-?? ???????? ????????, ??????? ????? ??????? ???? ?? ?????????
void CPlaneArranger::ProcessNodeAxisEvent(IArrangable * wn, CCursorMoveEventArgs & a)
{
	CPlaneAdapter * ad = (CPlaneAdapter *)wn->GetAdapter(this);

	if(	a.InputMovement.Viewport	!= null			&& 
		a.Capture					!= null			&&
		a.Capture->GetInputMessage()->Sender == 1	/ *&&
		ad->WNode->NormalizingEnabled				&&
		ad->WNode->ArrangingEnabled* /)
	{
		DebugBreak(); CRay r;//!!! = Operator->GetEye(a.InputMovement.Viewport)->PView->GetScanRay(a.InputMovement.VPosition.GetXY());
		
		CFloat3 p = Plane.GetRayIntersection(r);
		//CRay r = Area->Operator->GetEye(m.Viewport)->WView->GetScanRay(m.VPosition.GetXY());

		//float x		= tan(r.Direction.GetAngleToYZ()) * ad->Distance;
		//float y		= tan(r.Direction.GetAngleToXZ()) * ad->Distance;

		float x	= sin(r.Direction.GetAngleToYZ()) * a.Capture->GetIntersection()->GetDistance();
		float y	= sin(r.Direction.GetAngleToXZ()) * a.Capture->GetIntersection()->GetDistance();
		
		ad->SetNormalPosition(true, ad->Distance, p.x + CapturedX, p.y + CapturedY);
		
		//CMath::Bound(&h, -3000, +3000);

		wn->Arrange(ad->GetNormalTransformation(), CAnimation::No());
		//ad->WNode->RiseBeginEvent(EWNodeEvent_Arranging, CWNodeTransformationArgs());
		//ad->WNode->RiseEndEvents();
	}
}*/

void CPlaneArranger::Place(CArray<IArrangable *> & arr, CSize & s, IArrangable * a, CAnimation & ani)
{
	PlaceCentripetally3(arr, a, ani);
}

CArray<CRect> CPlaneArranger::GetInflatedRects(CArray<IArrangable *> & arr, IArrangable * wn, float spacing)
{
	float aw = wn->GetArea().W;
	float ah = wn->GetArea().H;
		
	CArray<CRect> rects;
	rects.reserve(arr.size());
	
	for(auto i : arr)
	{
		CPlaneAdapter * ad = (CPlaneAdapter *)i->GetAdapter(this);

		if(i == wn)
		{
			continue;
		}
	
		float w = i->GetArea().W + aw + spacing*2;
		float h = i->GetArea().H + ah + spacing*2;

		float x = ad->X - w/2;
		float y = ad->Y - h/2;
		
		rects.push_back(CRect(x, y, w, h));
	}
	return rects;
}
	
static bool PointSorter(CFloat3 & a, CFloat3 & b)
{
	return a.z < b.z;
}

static bool RectSorter(CRect & a, CRect & b)
{
	return (a.X+a.W/2)*(a.X+a.W/2) + (a.Y+a.H/2)*(a.Y+a.H/2) < (b.X+b.W/2)*(b.X+b.W/2) + (b.Y+b.H/2)*(b.Y+b.H/2);
}


static bool MinCornerDistanceSorter(CRect & a, CRect & b)
{
	return	min(
				min(a.GetLT().GetLengthSq(), a.GetLB().GetLengthSq()),
				min(a.GetRT().GetLengthSq(), a.GetRB().GetLengthSq())
			) 
			>
			min(
				min(b.GetLT().GetLengthSq(), b.GetLB().GetLengthSq()),
				min(b.GetRT().GetLengthSq(), b.GetRB().GetLengthSq())
			);
			
}

// ???????? ????? ???? ????????????? ???? ?? ?????????? rects ??????? ????????? ??? ??????????? ?? ??? ????? ?? ??????
// ? ? ?????????? ????????? ???????????? ??? ??????? ??????
void CPlaneArranger::PlaceCentripetally3(CArray<IArrangable *> & arr, IArrangable * wn, CAnimation & ani)
{
	float spacing = 30.f;

	//float aw = wn->GNode->GetOBB().GetWidth();
	//float ah = wn->GNode->GetOBB().GetHeight();
	
	CArray<CRect> rects = GetInflatedRects(arr, wn, spacing);
			
	std::sort(rects.begin(), rects.end(), MinCornerDistanceSorter);

	//ReportDebug(L"rects = %f", timer.GetTime());
	//timer.Start();
	
	float dmin = FLT_MAX;
	CFloat2 p(0, 0);
	
	for(auto i = rects.begin(); i != rects.end(); i++)
	{
		float l = i->GetLeft();
		float t = i->GetTop();
		float r = i->GetRight();
		float b = i->GetBottom();
		
		if((dmin < l*l+t*t && dmin < r*r+t*t && dmin < l*l+b*b && dmin < r*r+b*b) &&
			!((l < 0 && 0 < r) && (dmin > t*t || dmin > b*b)) &&
			!((b < 0 && 0 < t) && (dmin > l*l || dmin > r*r))
			)
		{
			continue;
		}

		
		if(dmin > l*l+t*t && !RectsContainPointWOB(rects, l, t))
		{
			p.x = l;
			p.y = t;
			dmin = l*l+t*t;
		}
		if(dmin > r*r+t*t && !RectsContainPointWOB(rects, r, t))
		{
			p.x = r;
			p.y = t;
			dmin = r*r+t*t;
		}
		if(dmin > l*l+b*b && !RectsContainPointWOB(rects, l, b))
		{
			p.x = l;
			p.y = b;
			dmin = l*l+b*b;
		}
		if(dmin > r*r+b*b && !RectsContainPointWOB(rects, r, b))
		{
			p.x = r;
			p.y = b;
			dmin = r*r+b*b;
		}
		
		if(l < 0 && 0 < r)
		{
			if(dmin > t*t && !RectsContainPointWOB(rects, 0, t))
			{
				p.x = 0;
				p.y = t;
				dmin = t*t;
			}
			if(dmin > b*b && !RectsContainPointWOB(rects, 0, b))
			{
				p.x = 0;
				p.y = b;
				dmin = b*b;
			}
		}

		if(b < 0 && 0 < t)
		{
			if(dmin > l*l && !RectsContainPointWOB(rects, l, 0))
			{
				p.x = l;
				p.y = 0;
				dmin = l*l;
			}
			if(dmin > r*r && !RectsContainPointWOB(rects, r, 0))
			{
				p.x = r;
				p.y = 0;
				dmin = r*r;
			}
		}
		
		for(CArray<CRect >::iterator j=i+1; j!=rects.end(); j++)
		{
			if(j->IsIntersectWOB(*i))
			{
				CRect is = j->Intersect(*i);

				float la = is.GetLeft();
				float ta = is.GetTop();
				float ra = is.GetRight();
				float ba = is.GetBottom();

				if(dmin > la*la + ta*ta && !i->ContainNoBorder(la, ta) && !j->ContainNoBorder(la, ta) && !RectsContainPointWOB(rects, la, ta))
				{
					p.x = la;
					p.y = ta;
					dmin = la*la + ta*ta;
				}
				if(dmin > la*la + ba*ba && !i->ContainNoBorder(la, ba) && !j->ContainNoBorder(la, ba) && !RectsContainPointWOB(rects, la, ba))
				{
					p.x = la;
					p.y = ba;
					dmin = la*la + ba*ba;
				}
				if(dmin > ra*ra + ta*ta && !i->ContainNoBorder(ra, ta) && !j->ContainNoBorder(ra, ta) && !RectsContainPointWOB(rects, ra, ta))
				{
					p.x = ra;
					p.y = ta;
					dmin = ra*ra + ta*ta;
				}
				if(dmin > ra*ra + ba*ba && !i->ContainNoBorder(ra, ba) && !j->ContainNoBorder(ra, ba) && !RectsContainPointWOB(rects, ra, ba))
				{
					p.x = ra;
					p.y = ba;
					dmin = ra*ra + ba*ba;
				}
			}
		}
	}
	
	CPlaneAdapter * ad = (CPlaneAdapter *)wn->GetAdapter(this);
	ad->SetNormalPosition(false, 0, p.x, p.y);	
}	

bool CPlaneArranger::RectsContainPointWOB(CArray<CRect > & rects, float x, float y)
{
	for(auto i : rects)
	{
		if(i.ContainNoBorder(x, y))
		{
			return true;
		}
	}
	return false;
}


CArray<CSortKey> CPlaneArranger::PrepareArrange(CArray<IArrangable *> & arr)
{
	for(auto i : arr)
	{
		CPlaneAdapter * ad = (CPlaneAdapter *)i->GetAdapter(this);
		if(ad == null)
		{
			ad = new CPlaneAdapter(this);
			i->SetAdapter(this, ad);
		}
/*			CArea & a = (*i)->GetArea();
		if(!a.IsReal())
		{
			throw CException(HERE, L"IArrangable`s area is not real");
		}*/
	}

	CArray<CSortKey> keys;
	for(auto i : arr)
	{
		keys.push_back(CSortKey(i));
	}
	return keys;
}

void CPlaneArranger::FinilizeArrange(CArray<IArrangable *> & arr, CAnimation & ani)
{
	for(auto ab : arr)
	{
		CPlaneAdapter * ad =  dynamic_cast<CPlaneAdapter *>(ab->GetAdapter(this));
		ad->Arranged = true;
		auto t = ad->GetNormalTransformation();
		ab->Arrange(t, ani);
		//wn->RiseBeginEvent(EWNodeEvent_Arranging, CWNodeTransformationArgs());
	}
}
		
void CPlaneArranger::ArrangeDefault(CArray<IArrangable *> & arr, CAnimation & ani, float w)
{
	if(arr.empty())
	{
		return;
	}

	CArray<CSortKey> keys = PrepareArrange(arr);

	//float w = tan(s->GetTotalFov()/2) * NormalDistance*2;

	float yt = 0.f;
	float yb = 0.f;

	int start	= 0;
	int end		= 0;

	float spacingX = 50.f;
	float spacingY = 50.f;

	do
	{
		float ymax	= 0.f;
		float ar	= 0.f;
		float al	= 0.f;

		FitHorizontally(keys, w, spacingX, start, end, ymax);

		ArrangeHorizontally(keys,
							GetNext(0, &yt, &yb, ymax, spacingY),
							spacingX,
							start, 
							end);

		start = end + 1;
	}
	while(end < int(arr.size()-1));

	FinilizeArrange(arr, ani);

}

void CPlaneArranger::ArrangeSortingV(CArray<IArrangable *> & arr, CAnimation & ani)
{
	CArray<CSortKey> keys = PrepareArrange(arr);

	arr.Sort([](const CSortKey & l, const CSortKey & r){	return l < r;	});

	float ar = 0; // first
	float al = 0; // second

	float yaw=0.f;

	int start=0;
	int end=0;

	while(end < int(arr.size()))
	{
		start = end;
		while(end < (int)arr.size()-1 && arr[end+1] == arr[end])
		{
			end++;
		}
		
		ArrangeVertically(	keys,
							GetNext(0, &ar, &al, FindMaxWidth(keys, start, end), 50.f),
							start,
							end);
		end++;
	}
	FinilizeArrange(arr, ani);
}

void CPlaneArranger::ArrangeSortingH(CArray<IArrangable *> & arr, CAnimation & ani)
{
	CArray<CSortKey> keys = PrepareArrange(arr);
	arr.Sort([](const CSortKey & l, const CSortKey & r){	return l < r;	});

	float hR = 0; // first
	float hL = 0; // second

	float yaw=0.f;

	int start=0;
	int end=0;

	while(end < int(arr.size()))
	{
		start = end;
		while(end < (int)arr.size()-1 && arr[end+1] == arr[end])
		{
			end++;
		}

		ArrangeHorizontally(keys,
							GetNext(0, &hL, &hR, FindMaxHeight(keys, start, end), 50.f),
							50,
							start,
							end);
		end++;
	}
	FinilizeArrange(arr, ani);
}

void CPlaneArranger::ArrangeSortingR(CArray<IArrangable *> & arr, CAnimation & ani)
{
	CArray<CSortKey> keys = PrepareArrange(arr);
	arr.Sort([](const CSortKey & l, const CSortKey & r){	return l < r;	});

	float ar = 0; // first
	float al = 0; // second

	float yaw=0.f;

	int start=0;
	int end=0;

	while(end < int(arr.size()))
	{
		start = end;
		while(end < (int)arr.size()-1 && arr[end+1] == arr[end])
		{
			end++;
		}
		
		ArrangeByDistance(	keys,
							GetNext(0, &ar, &al, FindMaxWidth(keys, start, end), 50.f),
							start,
							end);
		end++;
	}
	FinilizeArrange(arr, ani);
}

void CPlaneArranger::ArrangeHorizontally(CArray<CSortKey> & keys, float y, float spacingX, int start, int end)
{
	float xR = 0;
	float xL = 0;

	for(int i=start; i<=end; i++)
	{
		float w = keys[i].Arrangable->GetArea().W;
		((CPlaneAdapter *)keys[i].Arrangable->GetAdapter(this))->SetNormalPosition(false, 0, GetNext(0, &xR, &xL, w, spacingX), y);
	}
}	

void CPlaneArranger::ArrangeVertically(CArray<CSortKey> & keys, float yaw, int start, int end)
{
	float hU = 0;
	float hD = 0;
	float h;

	for(int i=start; i<=end; i++)
	{
		h = keys[i].Arrangable->GetArea().H;
		((CPlaneAdapter *)keys[i].Arrangable->GetAdapter(this))->SetNormalPosition(false, 0, yaw, GetNext(0, &hU, &hD, h, 50.f));
	}
}

void CPlaneArranger::ArrangeByDistance(CArray<CSortKey> & keys, float yaw, int start, int end)
{
	float dh = 0.f;

	for(int i=start; i<=end; i++)
	{
		((CPlaneAdapter *)keys[i].Arrangable->GetAdapter(this))->SetNormalPosition(false, dh, yaw, dh);
		dh += 200.f;
	}
}
/*

#pragma region Options
void CPlaneArranger::CreateOptions(IOptioner * o, IOptionLocation * parent)
{
	Optioner = o;
	OptionBlock = o->CreateBlock(L"Size", ModuleDescriptor->GetPath(L"PlaneAreaOptionsPanel.xrc"), L"WorldPlaneAreaOptionsPanel");
	OptionBlock->Inited			+= EventHandler(CPlaneArranger::OnOptionBlockInited);
	OptionBlock->DataCommited	+= EventHandler(CPlaneArranger::OnOptionBlockCommited);
	Optioner->AddBlock(OptionBlock);

	OptionLocation = o->CreateLocation(parent, L"Size", OptionBlock);
}

void CPlaneArranger::DeleteOptions()
{
	cleandelete(OptionLocation);

	Optioner->RemoveBlock(OptionBlock);
	cleandelete(OptionBlock);
}

void CPlaneArranger::OnOptionBlockInited()
{
	OptionBlock->SetSource(RootParameter);
	OptionBlock->Assign(L"DefaultDistance",	L"DefaultDistance");
}

void CPlaneArranger::OnOptionBlockCommited()
{
	Config->Save(RootParameter);
	ApplyParameters();
}
#pragma region*/


#include "stdafx.h"
#include "Camera.h"

using namespace uos;

CCamera::CCamera(CEngineLevel * l, CViewport * vp, CXon * p) : CEngineEntity(l)
{
	Name		= p->One(L"Name")->Get<CString>();
	Fov			= p->One(L"Fov")->AsFloat32();
	ZFar		= p->One(L"ZFar")->AsFloat32();
	ZNear		= p->One(L"ZNear")->AsFloat32();
	Viewport	= vp;
	ViewportFormula = [this]{ return CSize(Viewport->W, Viewport->H, 0); };
}

CCamera::CCamera(CEngineLevel * e, CViewport * vp, const CString & name, float fov, float znear, float zfar) : CEngineEntity(e)
{
	Name		= name;
	Viewport	= vp;
	Projection	= EProjection::Perspective;
	Fov			= fov;
	ZNear		= znear;
	ZFar		= zfar;
	ViewportFormula = [this]{ return CSize(float(Viewport->W), float(Viewport->H), 0); };
}

CCamera::CCamera(CEngineLevel * e, CViewport * vp, const CString & name, float znear, float zfar) : CEngineEntity(e)
{
	Name		= name;
	Viewport	= vp;
	Projection	= EProjection::Ortho;
	Fov			= 0;
	ZNear		= znear;
	ZFar		= zfar;
	ViewportFormula = [this]{ return CSize(Viewport->W, Viewport->H, 0); };
}

CCamera::~CCamera()
{
}

void CCamera::UseAffine()
{
	ViewFormula =	[this]()
					{
						return CMatrix::FromTransformation(Position, CQuaternion(Rotation), CFloat3(0.f), CFloat3(1.f)).GetInversed()/* * CMatrix::FromScaling(Viewport->Scaling.x, Viewport->Scaling.y, 1.f)*/;
					};
	UpdateViewProjection();
}

void CCamera::UseLookToLeftHand()
{
	ViewFormula =	[this]()
					{
						return CMatrix::FromLookToLH(Position, Direction, Up)/* * CMatrix::FromScaling(Viewport->Scaling.x, Viewport->Scaling.y, 1.f)*/;
					};
	UpdateViewProjection();
}

void CCamera::OnDiagnosticsUpdateStarted(CDiagnostic * d)
{
}

///void CCamera::MovePosition(float fbMove, float lrMove, float udMove)
///{
///	ApplyMove(Position + GetVelocity(fbMove, lrMove, udMove), Rotation);
///}

void CCamera::SetPosition(CFloat3 & pos)
{
	Position = pos;
	UpdateViewProjection();
}

void CCamera::SetRotation(CFloat3 & r)
{
	Rotation = r;
	UpdateViewProjection();
}

void CCamera::SetUp(CFloat3 & up)
{
	Up = up;
	UpdateViewProjection();
}

void CCamera::SetDirection(CFloat3 & dir)
{
	Direction = dir;
	UpdateViewProjection();
}

void CCamera::Rotate(CQuaternion & dr)
{
	Rotation = Rotation * dr;
	UpdateViewProjection();
}
///
///void CCamera::ApplyMove(CFloat3 & p, CFloat3 & r)
///{
///	Position = p;
///	Rotation = r;
///	Rotation.AdjustRotationToPiRange();
///
///	CalculateMatrixes();
///
///	Moved(this);
///}	

//void CCamera::LookAt(CFloat3 & p)
//{
//	D3DXMatrixLookAtLH(&ViewMatrix, &Position, &p, &CFloat3(0, 1, 0));
//	ViewMatrixInvers = ViewMatrix.GetInversed();
//
//
//	//auto t = ViewMatrix.Decompose();
//	//
//	//Rotation = t.Rotation;
//
//	//U.x		= ViewMatrix._11;
//	//U.y		= ViewMatrix._21;
//	//U.z		= ViewMatrix._31;
//	//
//	//Up.x	= ViewMatrix._12;
//	//Up.y	= ViewMatrix._22;
//	//Up.z	= ViewMatrix._32;
//	//
//	//Look.x	= ViewMatrix._13;
//	//Look.y	= ViewMatrix._23;
//	//Look.z	= ViewMatrix._33;
//			
//	Moved(this);
//}	

void CCamera::SetFov(float fov)
{
	Fov = fov;
	Modified(this);
}

///CFloat3 CCamera::GetVelocity(float fbMove, float lrMove, float udMove)
///{
///	CFloat3 fb;
///	CFloat3 lr;
///	CFloat3 vel;
///
///	CFloat3 up;
///	up.x	= ViewMatrix._12;
///	up.y	= ViewMatrix._22;
///	up.z	= ViewMatrix._32;
///	
///	CFloat3 look;
///	look.x	= ViewMatrix._13;
///	look.y	= ViewMatrix._23;
///	look.z	= ViewMatrix._33;
///
///
///	lr = up.Cross(look);
///	lr = lr.GetNormalized();
///
///	fb	= look*fbMove;
///	lr	= lr*lrMove;
///
///	return ((fb + lr - CFloat3(0.f, udMove, 0.f))) * -1.f;
///}

float CCamera::GetHFov()
{
	return Fov;
}

float CCamera::GetVFov()
{
	return Fov/Aspect;
}

float CCamera::GetAspect()
{
	return Aspect;
}

void CCamera::SetOffset(CFloat2 & o)
{
	Offset = o;
	UpdateViewProjection();
}

void CCamera::UpdateViewProjection()
{
	if(!Viewport || !ViewFormula || !ViewportFormula)
	{
		return;
	}

	ViewMatrix			= ViewFormula();
	ViewMatrixInvers	= ViewMatrix.GetInversed();

	auto s = ViewportFormula();

	Aspect = s.W/s.H;

	switch(Projection)
	{
		case EProjection::Perspective :
		{
			if(Fov < 0 || Fov > M_PI)
			{
				///throw CException(HERE, L"Wrong fov");
			}
			
			/*float h0 = 1/tan((Fov/Aspect)/2); 
			float w0 = h0 / Aspect;
			ZeroMemory(&ProjectionMatrix, sizeof(ProjectionMatrix));
			ProjectionMatrix._11 = w0;
			ProjectionMatrix._22 = h0;
			ProjectionMatrix._33 = Camera->ZFar / (Camera->ZFar - Camera->ZNear);
			ProjectionMatrix._34 = 1.0;
			ProjectionMatrix._43 = -Camera->ZFar * Camera->ZNear / (Camera->ZFar - Camera->ZNear);*/
			ProjectionMatrix = CMatrix::FromPerspectiveFovLH(Fov/Aspect, Aspect, ZNear, ZFar);

			// same as: ProjectionMatrix = ProjectionMatrix * CMatrix::FromPosition(Offset.x, Offset.y, 0) 
			ProjectionMatrix._31 = Offset.x; // x offset 
			ProjectionMatrix._32 = Offset.y; // y offset 
			break;
		}
		case EProjection::Ortho :
			ProjectionMatrix = CMatrix::FromOrthographicOffCenterLH(0, s.W, 0, s.H, ZNear, ZFar);
			break;
	}

	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;


	// Calculate near plane of frustum.
	Frustum[0].x = ViewProjectionMatrix._14 + ViewProjectionMatrix._13;
	Frustum[0].y = ViewProjectionMatrix._24 + ViewProjectionMatrix._23;
	Frustum[0].z = ViewProjectionMatrix._34 + ViewProjectionMatrix._33;
	Frustum[0].w = ViewProjectionMatrix._44 + ViewProjectionMatrix._43;
	Frustum[0].Normalize();

	// Calculate far plane of frustum.
	Frustum[1].x = ViewProjectionMatrix._14 - ViewProjectionMatrix._13; 
	Frustum[1].y = ViewProjectionMatrix._24 - ViewProjectionMatrix._23;
	Frustum[1].z = ViewProjectionMatrix._34 - ViewProjectionMatrix._33;
	Frustum[1].w = ViewProjectionMatrix._44 - ViewProjectionMatrix._43;
	Frustum[1].Normalize();

	// Calculate left plane of frustum.
	Frustum[2].x = ViewProjectionMatrix._14 + ViewProjectionMatrix._11; 
	Frustum[2].y = ViewProjectionMatrix._24 + ViewProjectionMatrix._21;
	Frustum[2].z = ViewProjectionMatrix._34 + ViewProjectionMatrix._31;
	Frustum[2].w = ViewProjectionMatrix._44 + ViewProjectionMatrix._41;
	Frustum[2].Normalize();

	// Calculate right plane of frustum.
	Frustum[3].x = ViewProjectionMatrix._14 - ViewProjectionMatrix._11; 
	Frustum[3].y = ViewProjectionMatrix._24 - ViewProjectionMatrix._21;
	Frustum[3].z = ViewProjectionMatrix._34 - ViewProjectionMatrix._31;
	Frustum[3].w = ViewProjectionMatrix._44 - ViewProjectionMatrix._41;
	Frustum[3].Normalize();

	// Calculate top plane of frustum.
	Frustum[4].x = ViewProjectionMatrix._14 - ViewProjectionMatrix._12; 
	Frustum[4].y = ViewProjectionMatrix._24 - ViewProjectionMatrix._22;
	Frustum[4].z = ViewProjectionMatrix._34 - ViewProjectionMatrix._32;
	Frustum[4].w = ViewProjectionMatrix._44 - ViewProjectionMatrix._42;
	Frustum[4].Normalize();

	// Calculate bottom plane of frustum.
	Frustum[5].x = ViewProjectionMatrix._14 + ViewProjectionMatrix._12;
	Frustum[5].y = ViewProjectionMatrix._24 + ViewProjectionMatrix._22;
	Frustum[5].z = ViewProjectionMatrix._34 + ViewProjectionMatrix._32;
	Frustum[5].w = ViewProjectionMatrix._44 + ViewProjectionMatrix._42;
	Frustum[5].Normalize();
}

CFloat3 CCamera::ProjectVertex(CFloat3 & in) // project to the viewport, not to the screen
{
	CFloat3 out;

	CFloat3 v = ViewProjectionMatrix.TransformCoord(in);

	out.x	= /*Viewport->X + */ ( 1.f + v.x ) * Viewport->W	/ 2.0f;
	out.y	= /*Viewport->Y + */ ( 1.f + v.y ) * Viewport->H	/ 2.0f;
	out.z	= Viewport->MinZ + v.z * (Viewport->MaxZ - Viewport->MinZ);
	return out;
}

CFloat2 CCamera::ProjectVertexXY(CFloat3 & in) // project to the viewport, not to the screen
{
	CFloat2 out;

	CFloat3 v = ViewProjectionMatrix.TransformCoord(in);

	out.x	= /*Viewport->X + */ ( 1.f + v.x ) * Viewport->W	/ 2.0f;
	out.y	= /*Viewport->Y + */ ( 1.f + v.y ) * Viewport->H	/ 2.0f;
	return out;
}

CFloat3 CCamera::UnprojectVertex(CFloat3 & in) // unproject from the viewport, not from the screen
{
	CFloat3 out;
	
	out.x = 2 * (in.x/* - Viewport->X*/)/Viewport->W - 1;
	out.y = 2 * (in.y/* - Viewport->Y*/)/Viewport->H - 1;
	out.z =		(in.z - Viewport->MinZ)/(Viewport->MaxZ - Viewport->MinZ);

	return ViewProjectionMatrix.GetInversed().TransformCoord(out);
}

CRay CCamera::Raycast(CFloat2 & p)
{
	CRay r;
	CFloat3 o;
	
	if(Projection == EProjection::Perspective)
	{
		r.Direction.x = (((2.f * p.x)/Viewport->W)	- 1.f)/ProjectionMatrix._11;   // [-1..1]
		r.Direction.y = (((2.f * p.y)/Viewport->H)	- 1.f)/ProjectionMatrix._22;
		r.Direction.z = 1.f;
		r.Direction = ViewMatrixInvers.TransformNormal(r.Direction);
		
		r.Origin	= Position - CFloat3(Offset.x, Offset.y, 0);
	}
	if(Projection == EProjection::Ortho)
	{
		r.Direction = ViewMatrixInvers.TransformNormal(CFloat3(0.f, 0.f, 1.f));
		r.Origin	= ViewMatrixInvers.TransformCoord(CFloat3(p.x, p.y, 0.f) - CFloat3(Offset.x, Offset.y, 0));
	}
	
	r.Direction = r.Direction.GetNormalized();

	return r;
}

CRay CCamera::Raycast()
{
	CRay r;
	CFloat3 o;
	
	if(Projection == EProjection::Perspective)
	{
		r.Direction = ViewMatrixInvers.TransformNormal(CFloat3(0.f, 0.f, 1.f));
		r.Origin	= Position - CFloat3(Offset.x, Offset.y, 0);
	}
	if(Projection == EProjection::Ortho)
	{
		r.Direction = ViewMatrixInvers.TransformNormal(CFloat3(0.f, 0.f, 1.f));
		r.Origin	= ViewMatrixInvers.TransformCoord(CFloat3(0) - CFloat3(Offset.x, Offset.y, 0));
	}
	
	r.Direction = r.Direction.GetNormalized();

	return r;
}


CRay CCamera::GetScanRayPCenterDelta(CFloat2 & p) 
{
	CRay s;

	s.Origin = Position;
	s.Direction.x =	((2.f * p.x)/Viewport->W)/ProjectionMatrix._11;
	s.Direction.y =	((2.f * p.y)/Viewport->H)/ProjectionMatrix._22;
	s.Direction.z =	1.f;
	
	s.Direction = s.Direction.GetNormalized();

	return s;
}

CMatrix & CCamera::GetViewProjectionMatrix()
{
	return ViewProjectionMatrix;
}

float CCamera::UnprojectZ(float x, float sx)
{
	if(Projection == EProjection::Perspective)
	{
		auto a = sx/(Viewport->W/2);
	
		return ProjectionMatrix._11 * x/a; // x * aspect / sx * tan(fovy * 0.5) 
	}
	throw CException(HERE, L"Wrong Projection");
}

float CCamera::UnprojectX(float z, float sx)
{
	if(Projection == EProjection::Perspective)
	{
		sx = sx/(Viewport->W/2);

		return z * sx / ProjectionMatrix._11; //
	}
	throw CException(HERE, L"Wrong Projection");
}

float CCamera::UnprojectY(float z, float sy)
{
	if(Projection == EProjection::Perspective)
	{
		sy = sy/(Viewport->H/2);

		return z * sy / ProjectionMatrix._22; //
	}
	throw CException(HERE, L"Wrong Projection");
}

bool CCamera::Contains(CFloat3 & p)
{
	for(auto & f : Frustum) 
	{
		if(f.Dot(CFloat4(p.x, p.y, p.z, 1)) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool CCamera::Contains(CFloat3 & a, CFloat3 & b, CFloat3 & c, bool backcull)
{
	//for(auto & f : Frustum) 
	//{
	//	if(f.Dot(CFloat4(a.x, a.y, a.z, 1)) < 0.f && f.Dot(CFloat4(b.x, b.y, b.z, 1)) < 0.f && f.Dot(CFloat4(c.x, c.y, c.z, 1)) < 0.f)
	//	{
	//		return false;
	//	}
	//}

	auto r = CRect(0, 0, Viewport->W, Viewport->H);
	
	auto pa = ProjectVertex(a); 
	auto pb = ProjectVertex(b); 
	auto pc = ProjectVertex(c);

	auto is = RectangleIntersectsTriangle(r, pa.ToXY(), pb.ToXY(), pc.ToXY());

	if(is && backcull)
	{
		pa.z = pb.z = pc.z = 0;

		return ((pb - pa).Cross(pc - pa)).z < 0;
	}
	else
		return is;
}

bool CCamera::RectangleIntersectsTriangle(CRect & r, CFloat2 & a, CFloat2 & b, CFloat2 & c)
{
	if(r.Contains(a) || r.Contains(b) || r.Contains(c))
	{
		return true;
	}

	auto lt = r.GetLT();
	auto lb = r.GetLB();
	auto rt = r.GetRT();
	auto rb = r.GetRB();

	if(CTriangle::ContainsPoint(a, b, c, lt) || CTriangle::ContainsPoint(a, b, c, lb) || CTriangle::ContainsPoint(a, b, c, rt) || CTriangle::ContainsPoint(a, b, c, rb))
	{
		return true;
	}

	// test each tri edge agains 2 rect diagonals insteed of 4 edges
	if(CFloat2::SegmentIntersection(a, b, lt, rb, null) ||
	   CFloat2::SegmentIntersection(b, c, lt, rb, null) ||
	   CFloat2::SegmentIntersection(c, a, lt, rb, null) ||
	   CFloat2::SegmentIntersection(a, b, lb, rt, null) ||
	   CFloat2::SegmentIntersection(b, c, lb, rt, null) ||
	   CFloat2::SegmentIntersection(c, a, lb, rt, null))
	{
		return true;
	}

	return false;
}

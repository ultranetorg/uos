#include "stdafx.h"
#include "Avatar.h"
//#include "Unit.h"
#include "World.h"

using namespace uos;

wchar_t * uos::ToString(ECardTitleMode a)
{
	static wchar_t * t[6] = {L"Null", L"No", L"Right", L"Left", L"Top", L"Bottom"};
	if(int(a) < 0 || int(a) > 5)
	{
		throw CException(HERE, L"EAvatarTitleMode wrong value");
	}
	return t[int(a)];
}

ECardTitleMode uos::ToAvatarTitleMode(const CString & name)
{
	if(name == L"No")		return ECardTitleMode::No; else
	if(name == L"Right")	return ECardTitleMode::Right; else
	if(name == L"Left")		return ECardTitleMode::Left; else
	if(name == L"Top")		return ECardTitleMode::Top; else
	if(name == L"Bottom")	return ECardTitleMode::Bottom; 
	else
		return ECardTitleMode::Null;
}

///////////////////////////////////////////////////////////////////////////////////

CAvatar::CAvatar(CWorld * l, CServer * s, CString const & name) : CElement(l, name), CNexusObject(s, name)
{
	World = l;
}

void CAvatar::SetEntity(CUol & e)
{
}

void CAvatar::DetermineSize(CSize & smax, CSize & s)
{
	if(!World->FullScreen)
	{
		if(!Size)
		{
			if(s)
			{
				Express(L"W", [s]{ return s.W; });
				Express(L"H", [s]{ return s.H; });
			}
			else
			{
				Express(L"W", [smax]{ return smax.W; });
				Express(L"H", [smax]{ return smax.H; });
			}
		}
	} 
	else
	{
		Express(L"W", [smax]{ return smax.W; });
		Express(L"H", [smax]{ return smax.H; });
	}
	UpdateLayout(CLimits::Empty, false);
}

//////////////////////////////////////////////////////////////////////////////////////

CModel::CModel(CWorld * l, CServer * s, ELifespan life, CString const & name) : CAvatar(l, s, name), Lifespan(life)
{
}

CModel::~CModel()
{
}

void CModel::Open(CUnit * a)
{
	Unit = a;
}

void CModel::Close(CUnit * a)
{
	Unit = null;
}

EPreferedPlacement CModel::GetPreferedPlacement()
{
	CArea * a = Unit;

	while(a)
	{
		if(PreferedPlacement.Contains(a->Name))
		{
			return PreferedPlacement(a->Name);
		}

		a = a->Parent;
	}

	if(PreferedPlacement.Contains(L""))
	{
		return PreferedPlacement(L"");
	}

	return EPreferedPlacement::Null;
}

CTransformation CModel::DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t)
{
	return t;
}
/// CStaticAvatar

CStaticAvatar::CStaticAvatar(CWorld * l, CServer * s, CXon * r, IMeshStore * mhs, IMaterialStore * mts, const CString & name) : CAvatar(l, s, name)
{
	LoadBasic(r, mhs, mts);
	
	if(!Size.IsReal())
	{
		throw CException(HERE, L"Bad Area");
	}

	auto size = Size;

	Express(L"Size", [size](auto){ return size; });

	auto am = new CSolidRectangleMesh(&l->Engine->EngineLevel);
	am->Generate(0, 0, Size.W, Size.H);
	Active->SetMesh(am);
	am->Free();
}

CStaticAvatar::CStaticAvatar(CWorld * l, CServer * s, CElement * e, const CString & name) : CAvatar(l, s, name)
{
	Name			= e->Name;
	Enabled			= e->Enabled;
	Transformation	= e->Transformation;
	Size			= e->Size;
	Slimits			= e->Slimits;

	sh_free(Visual);
	Visual = e->Visual->Clone();

	auto size = Size;

	Express(L"Size", [size](auto){ return size; });

	auto am = new CSolidRectangleMesh(&l->Engine->EngineLevel);
	am->Generate(0, 0, Size.W, Size.H);
	Active->SetMesh(am);
	am->Free();
}

CStaticAvatar::~CStaticAvatar()
{
}


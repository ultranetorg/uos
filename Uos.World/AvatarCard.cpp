#include "StdAfx.h"
#include "AvatarCard.h"

using namespace uos;

CAvatarCard::CAvatarCard(CWorld * l, const CString & name) : CCard(l, name)
{
}

CAvatarCard::~CAvatarCard()
{
	OnDependencyDestroying(Avatar);
	OnDependencyDestroying(Entity);

	if(Avatar)
	{
		Level->DestroyAvatar(Avatar);
	}
}

void CAvatarCard::SetAvatar(CUol & a, CString const & dir)
{
	Type = a.Parameters(L"type");
	Avatar = Level->CreateAvatar(a, dir);

	if(Avatar)
	{
		Avatar->Transform(0, 0, Z_STEP);
		Avatar->Destroying += ThisHandler(OnDependencyDestroying);

		SetFace(Avatar);

		//Visual->SetInheritableMaterial(null);
		//UpdateLayout();
	}
	else
	{
		///Visual->SetInheritableMaterial(Service->GetOfflineMaterial());
	}
}

void CAvatarCard::SetEntity(CUol & e)
{
	auto protocol = Level->Nexus->Connect<IAvatarProtocol>(this, e, AVATAR_PROTOCOL);
		
	if(protocol)
	{
		Entity = protocol->GetEntity(e);
		Entity->Destroying += ThisHandler(OnDependencyDestroying);
		Entity->Retitled += ThisHandler(OnTitleChanged);

		OnTitleChanged(Entity);
	}
}

void CAvatarCard::OnDependencyDestroying(CNexusObject * o)
{
	if(Avatar && o == Avatar)
	{
		SetFace(null);
		Avatar->Destroying -= ThisHandler(OnDependencyDestroying);
		//Avatar = null;
	}

	if(Entity && o == Entity)
	{
		Entity->Retitled -= ThisHandler(OnTitleChanged);
		Entity->Destroying -= ThisHandler(OnDependencyDestroying);
		Entity = null;
	}
}

void CAvatarCard::OnTitleChanged(CWorldEntity *)
{
	Title = Entity->Title;
	if(Text)
	{
		Text->SetText(Title);
		PropagateLayoutChanges(Text);
	}
}

void CAvatarCard::SetMetrics(CAvatarMetrics & m)
{
	Metrics = m;

	Avatar->DetermineSize(m.FaceSize, m.FaceSize);

	Metrics.FaceSize = Avatar->Size;

	Avatar->Express(L"W", [this]{ return Metrics.FaceSize.W; });
	Avatar->Express(L"H", [this]{ return Metrics.FaceSize.H; });
	Avatar->Express(L"M", [this]{ return Metrics.FaceMargin; });
}

void CAvatarCard::Save(CXon * x)
{
	x->Add(L"Title")->Set(Title);
	x->Add(L"TitleMode")->Set(ToString(TitleMode));
	Metrics.FaceSize = Face->Size;
	Metrics.Save(x->Add(L"Metrics"));
}

void CAvatarCard::Load(CXon * x)
{
	Title = x->Get<CString>(L"Title");
	SetTitleMode(ToAvatarTitleMode(x->Get<CString>(L"TitleMode")));
	SetMetrics(CAvatarMetrics(x->One(L"Metrics")));
}

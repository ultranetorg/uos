#include "StdAfx.h"
#include "EngineLevel.h"

using namespace uos;

CEngineEntity::CEngineEntity(CEngineLevel * l)
{
	Level = l;
}

CEngineEntity::~CEngineEntity()
{
}

void CEngineEntity::VerifyWithWarning(HRESULT hr)
{
	if(hr != S_OK)
	{
		Level->Log->ReportWarning(this, CEngineLevel::GetErrorDescription(hr).c_str());
	}
}

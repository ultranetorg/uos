#include "stdafx.h"
#include "Shared.h"

using namespace uos;

CShared * CShared::Take()
{
	Refs++;
	return this;
}

void CShared::Free()
{
	Refs--;
	if(Refs == 0)
	{
		delete (this);
	}
}

int CShared::GetRefs()
{
	return Refs;
}

CShared::~CShared()
{
}

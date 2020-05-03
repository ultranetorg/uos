#include "stdafx.h"
#include "Config.h"
#include "Nexus.h"

using namespace uos;

CConfig::CConfig(CLevel2 * l, CString & durl, CString & curl)
{
	Level = l;
	DefaultUri = durl;
	CustomUri = curl;

	auto ds = Level->Nexus->Storage->OpenReadStream(durl);
	DefaultDoc = new CTonDocument(CXonTextReader(ds));
	Level->Nexus->Storage->Close(ds);

	if(Level->Nexus->Storage->Exists(curl))
	{
		auto ds = Level->Nexus->Storage->OpenReadStream(durl);
		auto cs = Level->Nexus->Storage->OpenReadStream(curl);

		Root = new CTonDocument(CXonTextReader(ds), CXonTextReader(cs));
		
		Level->Nexus->Storage->Close(ds);
		Level->Nexus->Storage->Close(cs);
	}
	else
	{
		auto s = Level->Nexus->Storage->OpenReadStream(durl);
		Root = new CTonDocument(CXonTextReader(s));
		Level->Nexus->Storage->Close(s);
	}

}

CConfig::~CConfig()
{
	delete Root;
	delete DefaultDoc;
}

void CConfig::Save()
{
	auto cs = Level->Nexus->Storage->OpenWriteStream(CustomUri);

	Root->Save(&CXonTextWriter(cs, false), DefaultDoc);

	Level->Nexus->Storage->Close(cs);
}

CXon * CConfig::GetRoot()
{
	return Root;
}

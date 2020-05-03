#include "StdAfx.h"
#include "FontFactory.h"

using namespace uos;

CFontFactory::CFontFactory(CEngineLevel * l, CDirectSystem * ge)
{
	Level = l;
	GraphicEngine = ge;
}

CFontFactory::~CFontFactory()
{
	for(auto i : Fonts)
	{
		delete i;
	}	
}

CFont * CFontFactory::GetFont(const CString & family, float size, bool bold, bool italic)
{
	auto f = Fonts.Find([family, size, bold, italic](CFont * i)
													{
														return	i->Family == family
																&& i->Size == size
																&& i->IsBold == bold
																&& i->IsItalic == italic;
													});

	if(!f)
	{
		f = new CFont(GraphicEngine, family, size, bold, italic);
		Fonts.push_back(f);
	}
	return f;
}

CFont * CFontFactory::GetFont(CFontDefinition & d)
{
	return GetFont(d.Family, d.Size, d.Bold, d.Italic);
}

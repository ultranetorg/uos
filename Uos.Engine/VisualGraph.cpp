#include "StdAfx.h"
#include "VisualGraph.h"

using namespace uos;

CVisualGraph::CVisualGraph(CEngineLevel * l, const CString & name) : CEngineEntity(l)
{
	Name = name;
}

CVisualGraph::~CVisualGraph()
{
}

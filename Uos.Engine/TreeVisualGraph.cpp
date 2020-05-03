#include "StdAfx.h"
#include "TreeVisualGraph.h"
#include "VisualSpace.h"

using namespace uos;

CTreeVisualGraph::CTreeVisualGraph(CEngineLevel * l, CDirectPipelineFactory * pf, const CString & name) : CVisualGraph(l, name)
{
	PipelineFactory = pf;

	Diagnostic = Level->Core->Supervisor->CreateDiagnostics(name + L" - TreeVisualGraph");
	Diagnostic->Updating += ThisHandler(OnDiagnosticsUpdate);

	DiagGrid.AddColumn(L"Name");
	DiagGrid.AddColumn(L"Refs");
	DiagGrid.AddColumn(L"Space");
	DiagGrid.AddColumn(L"Material");
	DiagGrid.AddColumn(L"Status");
	///DiagGrid.AddColumn(L"View");
	DiagGrid.AddColumn(L"Clipping");
	#ifdef _DEBUG
	DiagGrid.AddColumn(L"Position");
	DiagGrid.AddColumn(L"Rotation");
	DiagGrid.AddColumn(L"Scale");
	DiagGrid.AddColumn(L"SS Pos");
	#endif
	DiagGrid.AddColumn(L"Visual BB (in the node CS)");
}
	
CTreeVisualGraph::~CTreeVisualGraph()
{
	Diagnostic->Updating -= ThisHandler(OnDiagnosticsUpdate);
}

void CTreeVisualGraph::OnDiagnosticsUpdate(CDiagnosticUpdate & u)
{
	DiagGrid.Clear();

	std::function<void(CVisualSpace * s, CVisual *, const CString &)>dumpNode
	= [this, &dumpNode, &u](auto s, auto n, auto & tab)
	{
		if(Diagnostic->ShouldProceed(u, DiagGrid.GetSize()))
		{
			auto & r = DiagGrid.AddRow();

			if(Diagnostic->ShouldFill(u, DiagGrid.GetSize()))
			{
				r.SetNext(L"%p %s%s", n, tab, n->GetName());
				r.SetNext(L"%d", n->GetRefs());
				r.SetNext(s->Name);
				r.SetNext(n->Material ? n->Material->Name : L"");
				r.SetNext(n->GetStatus());
				///r.SetNext(n->GetActualView() ? n->GetActualView()->GetName() : L"");
				r.SetNext(n->GetClippingStatus());
				#ifdef _DEBUG
				r.SetNext(n->_Decomposed.Position.ToNiceString());
				r.SetNext(n->_Decomposed.Rotation.ToNiceString());
				r.SetNext(n->_Decomposed.Scale.ToNiceString());
				r.SetNext(n->_SSPosition.ToNiceString());
				#endif
				r.SetNext(n->GetAABB().ToNiceString());
			}

			for(auto i : n->Nodes)
			{
				dumpNode(s, i, tab + L"  ");
			}
		}
	};

	for(auto & s : Spaces)
	{
		for(auto v : s.second)
		{
			if(!v->Parent)
			{
				dumpNode(s.first, v, L""); 
			}
		}
	}

	Diagnostic->Add(u, DiagGrid);
}

void CTreeVisualGraph::AddNode(CVisualSpace * s, CVisual * v)
{
	Spaces(s).Add(v);
}

void CTreeVisualGraph::RemoveNode(CVisualSpace * s, CVisual * v)
{
	Spaces(s).Remove(v);
}

void CTreeVisualGraph::AddSpace(CVisualSpace * s)
{
	Spaces[s];
}

void CTreeVisualGraph::RemoveSpace(CVisualSpace * s)
{
	Spaces.Remove(s);
}

bool CTreeVisualGraph::Belongs(CVisualSpace * s, CVisual * root, CVisual * v)
{
	if(!Spaces(s).Contains(root))
	{
		throw CException(HERE, L"Root <-> VisualSpace error");
	}

	auto p = v;

	while(p && !IsRoot(p))
	{
		p = p->Parent;
	}

	return p == root;
}

bool CTreeVisualGraph::IsRoot(CVisual * v)
{
	for(auto & i : Spaces)
	{
		if(i.second.Contains(v))
		{
			return true;
		}
	}

	return false;
}

void CTreeVisualGraph::AssignPipeline(CVisual * v, CMap<CDirectPipeline *, int> & pipelines)
{
	if(v->Pipeline)
	{
		v->Pipeline->Visuals.Remove(v);
	}

	auto p = PipelineFactory->GetPipeline(v->Material->Shader);
	p->Visuals.push_back(v);

	if(!pipelines.Contains(p))
	{
		pipelines[p]++;
	}

	if(v->Pipeline && v->Pipeline != p)
	{
		v->Pipeline->Free();
	}
	if(p && v->Pipeline == p)
	{
		p->Free();
	}
		
	v->Pipeline = p;

	v->ShaderChanged = false;
		
	///auto n = v;
	///while(n)
	///{
	///	if(n->InheritableMaterial)
	///	{
	///		n->InheritableMaterial->BuildShaderCode(&c, v->Mesh);
	///	}
	///	n = n->Parent;
	///}

}

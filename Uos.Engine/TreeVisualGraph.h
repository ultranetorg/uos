#pragma once
#include "VisualGraph.h"
#include "PipelineFactory.h"

namespace uos
{
	class UOS_ENGINE_LINKING CTreeVisualGraph : public CVisualGraph
	{
		public:
			CMap<CVisualSpace *, CRefList<CVisual *>>	Spaces;
			CDiagnostic *								Diagnostic;
			CDiagGrid									DiagGrid;
			CDirectPipelineFactory *					PipelineFactory;

			UOS_RTTI
			CTreeVisualGraph(CEngineLevel * l, CDirectPipelineFactory * pf, const CString & name);
			~CTreeVisualGraph();

			void										AddNode(CVisualSpace * s, CVisual * v) override;
			void										RemoveNode(CVisualSpace * s, CVisual * v) override;
			
			void										AddSpace(CVisualSpace * s) override;
			void										RemoveSpace(CVisualSpace * s) override;
			
			void										OnDiagnosticsUpdate(CDiagnosticUpdate & a);

			bool										Belongs(CVisualSpace * s, CVisual * root, CVisual * v);
			bool										IsRoot(CVisual * v);

			void										AssignPipeline(CVisual * v, CMap<CDirectPipeline *, int> & pipelines);

			/*
			template<typename F> void Enumerate(CSpace * s, F f)
			{
				std::function<void(CVisual *)>	e = [s, &f, &e](auto v)
				{
					if(!v->Space || v->Space->Space == s)
					{
						f(v);
						for(auto i : v->Nodes)
						{
							e(s, i);
						}
					}
				};

				for(auto i : Spaces[s].Nodes)
				{
					e(i);
				}
			}*/
	};
}
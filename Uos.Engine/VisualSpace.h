#pragma once
#include "EngineLevel.h"
#include "View.h"

namespace uos
{
	class CVisualGraph;

	class UOS_ENGINE_LINKING CVisualSpace : public CEngineEntity
	{
		public:
			CString										Name;
			CVisualSpace *								Parent = null;
			CList<CVisualGraph *>						Graphs;
			CList<CVisualSpace *>						Spaces;
			CMap<CViewport *, CPerformanceCounter>		PcRendering;
			CString										ParentName;
			CMatrix										Matrix = CMatrix::Identity;
			CView *										View = null;

			UOS_RTTI
			CVisualSpace(CEngineLevel * l, const CString & name);
			~CVisualSpace();
			
			void										AddFront(CVisualSpace * s);
			void										AddBack(CVisualSpace * s);
			void										AddBefore(CVisualSpace * s, CVisualSpace * before);
			void										AddAfter(CVisualSpace * s, CVisualSpace * after);
			void										Insert(CVisualSpace * s, CList<CVisualSpace *>::iterator pos);
			void										Remove(CVisualSpace * s);

			void										AddGraph(CVisualGraph * g);
			void										RemoveGraph(CVisualGraph * g);

			CVisualSpace *								Find(const CString & name);
			bool										IsDescedant(CVisualSpace * s);
			bool										IsUnder(CVisualSpace * parent);
			
			void										SetView(CView * v);
			CView *										GetView();
			CView *										GetActualView();
			
			void										Save(CXon * n);
			void										Load(CXon * n);
	};
}

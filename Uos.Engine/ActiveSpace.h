#pragma once
#include "Active.h"
#include "View.h"

namespace uos
{
	class CActiveGraph;

	class UOS_ENGINE_LINKING CActiveSpace : public virtual CShared
	{
		public:
			CString										Name;
			CString										ParentName;
			CActiveSpace *								Parent = null;
			CList<CActiveSpace *>						Spaces;
			CPerformanceCounter							PcRendering;
			CList<CActiveGraph *>						Graphs;
			CMatrix										Matrix = CMatrix::Identity;
			CView *										View = null;

			CActiveSpace(const CString & n);
			~CActiveSpace();
			
			void										AddFront(CActiveSpace * s);
			void										AddBack(CActiveSpace * s);
			void										AddBefore(CActiveSpace * s, CActiveSpace * p);
			void										AddAfter(CActiveSpace * s, CActiveSpace * p);
			void										Insert(CActiveSpace * s, CList<CActiveSpace *>::iterator pos);
			void										Remove(CActiveSpace * s);

			CActiveSpace *								Find(const CString & name);
			bool										IsDescedant(CActiveSpace * s);
			bool										IsUnder(CActiveSpace * parent);
			
			void										SetView(CView * v);
			CView *										GetView();
			CView *										GetActualView();

			void										Save(CXon * n);
			void										Load(CXon * n);

			void										AddGraph(CActiveGraph * g);
			void										RemoveGraph(CActiveGraph * g);
		
			template<class P> CActiveSpace * FindAncestor(P p)
			{
				auto n = this;

				while(n && !p(n))
				{
					n = n->Parent;
				}

				return n;
			}

	};
}

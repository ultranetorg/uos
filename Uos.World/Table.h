#pragma once
#include "Stack.h"

namespace uos
{
	class UOS_WORLD_LINKING CTableCell : public CStack
	{
		public:
			EXAlign										XAlign = EXAlign::Left;
			EYAlign										YAlign = EYAlign::Top;
			CExpressionList								MinimalExpressions;

			UOS_RTTI
			CTableCell(CWorldLevel * l, CStyle * s, CString const & name = GetClassName()) : CStack(l, s, name)
			{
				Express(L"C",	[this](auto apply)
								{
									for(auto i : Nodes)
									{
										i->UpdateLayout(Climits, apply);
									}
									return CalculateSize(Nodes);
								});
			}

			//using CStack::Align;

			CFloat2 Evaluate(bool apply)
			{
				W = H = IW = IH = NAN;

				CFloat2 q;
				
				Expressions.Execute(apply);

				O.x = M.LF + B.LF + P.LF;
				O.y = M.BM + B.BM + P.BM;
				q.x = M.RT + B.RT + P.RT;
				q.y = M.TP + B.TP + P.TP;

				if(Expressions.Contains(&W))	IW = W - (O.x + q.x); else 
				if(Expressions.Contains(&IW))	W = IW + (O.x + q.x);

				if(Expressions.Contains(&H))	IH = H - (O.y + q.y); else 
				if(Expressions.Contains(&IH))	H = IH + (O.y + q.y);

				return q;
			}

			void UpdateLayoutMinimal() 
			{
				Slimits = CLimits::Empty;
				C = CSize::Nan;

				Express(L"IW", [this]{ return C.W; });
				Express(L"IH", [this]{ return C.H; });

				Evaluate(false);

				Climits = CLimits::Empty;
				C = Expressions.Sizes[&C](false);
			
				Evaluate(false);
			
				Size = CSize(W, H, 0);
			}

			void UpdateLayoutActual(CLimits const & l, bool apply)
			{
				Slimits = l;
				C = CSize::Nan;

				Express(L"IW", [this]{ return C.W; });
				Express(L"IH", [this]{ return C.H; });

				auto q = Evaluate(false);

				CSize s;
				s.W = max(0, (isfinite(W) ? W : l.Smax.W) - (O.x + q.x));
				s.H = max(0, (isfinite(H) ? H : l.Smax.H) - (O.y + q.x));

				Climits = {s, s};
				C = Expressions.Sizes[&C](apply);

				Evaluate(false);

				Size = CSize(W, H, 0);
			}

			void UpdateLayoutAlign(float w, float h)
			{
				Express(L"W", [w]{ return w; });
				Express(L"H", [h]{ return h; });

				Evaluate(true);

				Size = CSize(W, H, 0);

				Align();
			}
	};

	class UOS_WORLD_LINKING CTable : public CElement
	{
		public:
			CFloat2										Spacing = CFloat2(0.f);
			CArray<CArray<CTableCell *>>				Cells;
			CStyle *									Style;

			UOS_RTTI
			CTable(CWorldLevel * l, CStyle * s, CString const & name = GetClassName());
			~CTable();
			
			CTableCell *								GetCell(int r, int c);
			virtual void								LoadNested(CStyle * s, CXon * n, std::function<CElement *(CXon *, CElement *)> & load) override;
			
			void virtual								LoadProperties(CStyle * s, CXon * n) override;
	};
}

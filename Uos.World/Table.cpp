#include "stdafx.h"
#include "Table.h"

using namespace uos;

CTable::CTable(CWorldLevel * l, CStyle * s, CString const & name /*= GetClassName()*/) : CElement(l, name)
{
	Style = s;
	Express(L"C",	[this](auto apply)
					{
						auto l = Climits;

						CArray<float> rows(Cells.Count(), 0.f);
						CArray<float> cols(Cells.Max([](auto & a){ return a.Count(); }).Count(), 0.f);

						float ws = max(0, Spacing.x * (cols.size() - 1));
						float hs = max(0, Spacing.y * (rows.size() - 1));

						for(auto r = 0; r < Cells.Count(); r++)
						{
							for(auto c = 0; c < Cells[r].Count(); c++)
							{
								auto n = Cells[r][c];
								n->UpdateLayoutMinimal();

								cols[c] = max(cols[c], n->Size.W);
								rows[r] = max(rows[r], n->Size.H);
							}
						}

						auto w = cols.Sum() + ws;
						auto h = rows.Sum() + hs;

						auto wmax = max(w, l.Smax.W);
						auto hmax = max(h, l.Smax.H);


						CArray<float> acols(cols.size(), 0);
						CArray<float> arows(rows.size(), 0);

						float fh = hmax - h;

						for(auto r = 0; r < Cells.Count(); r++)
						{
							float fw = wmax - w;

							for(auto c = 0; c < Cells[r].Count(); c++)
							{
								auto n = Cells[r][c];

								CLimits ll;
								ll.Smax = CSize(cols[c] + fw, rows[r] + fh, 0);
								ll.Pmax = CSize(wmax - ws, hmax - hs, 0);

								n->UpdateLayoutActual(ll, apply);

								acols[c] = max(acols[c], n->Size.W);
								arows[r] = max(arows[r], n->Size.H);

								fw -= max(0, n->Size.W - cols[c]);
							}

							fh -= max(0, arows[r] - rows[r]);
						}

						w = acols.Sum() + ws;
						h = arows.Sum() + hs;

						if(apply)
						{
							float x = 0;
							float y = h;
	
							for(auto r = 0; r < Cells.Count(); r++)
							{
								for(auto c = 0; c < Cells[r].Count(); c++)
								{
									Cells[r][c]->UpdateLayoutAlign(acols[c], arows[r]);
									Cells[r][c]->Transform(x, y - arows[r], Z_STEP);
									x += (Spacing.x + acols[c]);
								}
								x = 0;
								y -= (Spacing.y + arows[r]);
							}
						}

						return CSize(w, h, 0);
					});
}

CTable::~CTable()
{
}

CTableCell * CTable::GetCell(int r, int c)
{
	if(r >= Cells.Count())
	{
		Cells.resize(r+1);
	}

	if(c >= Cells[r].Count())
	{
		Cells[r].resize(c+1);
	}

	auto cell = Cells[r][c];
	
	if(!cell)
	{
		cell = new CTableCell(Level, Style);
		Cells[r][c] = cell;
		AddNode(cell);
		cell->Free();
	}

	return cell;
}

void CTable::LoadNested(CStyle * s, CXon * n, std::function<CElement *(CXon *, CElement *)> & load)
{
	for(auto i : n->Children)
	{
		auto p = i->Name.Split(L",");

		auto c = GetCell(CInt32(p[0]), CInt32(p[1]));

		load(i, c);
		//c->LoadProperties(s, &i->Get<CXon>());
		//
		//for(auto j : i->Children)
		//{
		//	auto wn = load(j);
		//	c->AddNode(wn);
		//	wn->Free();
		//}
	}
}

void CTable::LoadProperties(CStyle * s, CXon * n)
{
	__super::LoadProperties(s, n);

	for(auto i : n->Children)
	{
		if(i->Name == L"Spacing")	Spacing = CFloat2(i->Get<CString>()); 
	}
}

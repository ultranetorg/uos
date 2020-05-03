#include "StdAfx.h"
#include "Diagnostic.h"

using namespace uos;

CDiagnostic::CDiagnostic(const CString & name)
{
	Name = name;
}

CDiagnostic::~CDiagnostic()
{
}

CString & CDiagnostic::GetName()
{
	return Name;
}

void CDiagnostic::Clear()
{
	//Lines.reserve(100);
	//Lines.clear();
	Lines = 0;
}

int CDiagnostic::Size()
{
	return Lines;
}

void CDiagnostic::Append(const CString & t, DWORD c)
{
	//if(!Lines.back().empty() && Lines.back().back().Color == c)
	//{
	//	Lines.back().back().Text += t;
	//}
	//else
	//	Lines.back().push_back(CDiagElement(t, c));
	Appended(const_cast<CString &>(t), c);
}

void CDiagnostic::Add(CDiagnosticUpdate & u, CDiagGrid & g)
{
	DWORD lcolor = RGB(204, 204, 0);

	CArray<CString> f(g.Columns.size());
	for(auto i = 0U; i < g.Columns.size(); i++)
	{
		f[i] = CString::Format(g.Columns[i].Format, g.Columns[i].MaxWidth);
	}

	if(Size() <= u.Offset + u.Count)
		Add(L"");
	else
		return;

	for(auto i = 0U; i < g.Columns.size(); i++)
	{
		Append(CString::Format(f[i], g.Columns[i].Text));
		Append(L"\x2502", lcolor);
	}

	if(Size() <= u.Offset + u.Count)
		Add(L"");
	else
		return;

	for(auto i = 0U; i < g.Columns.size(); i++)
	{
		Append(CString(g.Columns[i].MaxWidth+2, L'\x2500'), lcolor);
		Append(L"\x253C", lcolor);
	}

	for(int r =0; r < g.Used; r++)
	{
		if(Size() <= u.Offset + u.Count)
			Add(L"");
		else
			return;

		for(auto c = 0U; c < g.Columns.size(); c++)
		{
			Append(CString::Format(f[c], g.Rows[r].Cells[c]));
			Append(L"\x2502", lcolor);
		}
	}
}

bool CDiagnostic::ShouldProceed(CDiagnosticUpdate & u, int s)
{
	auto total = Size() + s;
	return total + 1 <= u.Offset + u.Count;
}

bool CDiagnostic::ShouldFill(CDiagnosticUpdate & u, int s)
{
	auto total = Size() + s;
	return total > u.Offset;
}

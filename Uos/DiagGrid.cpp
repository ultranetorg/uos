#include "StdAfx.h"
#include "DiagGrid.h"

using namespace uos;

void CDiagGridRow::SetValue(int c, const CString & value)
{
	Cells[c] = value;
	Grid->Columns[c].MaxWidth = max(Grid->Columns[c].MaxWidth, (int)value.size());
}

void CDiagGridRow::SetNext(const CString & value)
{
	Cells[NextCell] = value;
	Grid->Columns[NextCell].MaxWidth = max(Grid->Columns[NextCell].MaxWidth, (int)value.size());
	NextCell++;
}

////////////

CDiagGrid::CDiagGrid()
{
	Columns.reserve(10);
}

CDiagGrid::~CDiagGrid()
{
}

void CDiagGrid::Clear()
{
	Used = 0;
	for(auto & i : Rows)
	{
		i.NextCell = 0;
	}
}

CDiagGridColumn & CDiagGrid::AddColumn(const CString & name, const CString & f)
{
	Columns.push_back(CDiagGridColumn());
	auto & c = Columns.back();
	c.Text = name;
	c.Format = f;
	c.MaxWidth = (int)name.size();
	return c;
}
	
CDiagGridRow & CDiagGrid::AddRow()
{
	if(Used < Rows.Count())
	{
		return Rows[Used++];
	}
	else
	{
		Used++;
		Rows.push_back(CDiagGridRow(this));

		auto & r = Rows.back();
		r.Cells.resize(Columns.size());
		return r;
	}
}


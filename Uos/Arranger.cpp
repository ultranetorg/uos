#include "StdAfx.h"
#include "Arranger.h"

using namespace uos;

CArranger::CArranger()
{
}

CArranger::~CArranger()
{
}

float CArranger::GetNext(float c, float * p, float * n, float v, float spacing)
{
	float o;
	if(*p - c == *n - c)
	{
		o = c;
		*p += (v/2 + spacing);
		*n -= (v/2 + spacing);
	}
	else if(abs(*p - c) <= abs(*n - c)) // by default we increment "a"
	{
		o = *p + v/2.f;
		*p += (v + spacing);
	}
	else//(abs(a) > abs(b))
	{
		o = *n - v/2.f;
		*n -= (v + spacing);
	}
	return o;
}

float CArranger::FindMaxWidth(CArray<CSortKey> & ads, int start, int end)
{
	float maxX = 0.f;
	for(int i=start; i<=end; i++)
	{
		float w = ads[i].Arrangable->GetArea().W;
		if(w > maxX)
		{
			maxX = w;
		}
	}
	return maxX;
}

float CArranger::FindMaxHeight(CArray<CSortKey> & ads, int start, int end)
{
	float maxY = 0.f;
	for(int i=start; i<=end; i++)
	{
		float h = ads[i].Arrangable->GetArea().H;
		if(h > maxY)
		{
			maxY = h;
		}
	}
	return maxY;
}

void CArranger::FitHorizontally(CArray<CSortKey> & ads, float total, float spacing, int start, int & end, float & maxy)
{
	float ar = 0; // first
	float al = 0; // second
		
	maxy = 0.f;
	end = start;

	for(int i=start; i<int(ads.size()); i++)
	{
		float w = ads[i].Arrangable->GetArea().W;
		float h = ads[i].Arrangable->GetArea().H;

		GetNext(0, &ar, &al, w, spacing);
		
		if(fabs(al) > total/2 && fabs(ar) > total/2) // value exeeded @total from both sides, only first item it can be so we will iclude only it
		{
			maxy = h;
			return;
		}
		
		if(fabs(al) > total/2 || fabs(ar) > total/2) // value exeeded @total from one side , exculde it
		{
			end--;
			return;
		}

		if(h > maxy)
		{
			maxy = h;
		}
		end++;
	}
		
	if(end > int(ads.size()-1))
	{
		end = (int)ads.size()-1;
	}
}

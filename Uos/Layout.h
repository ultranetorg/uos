#pragma once
#include "String.h"

namespace uos
{
	enum class EXAlign
	{
		Null, Left, Center, Right
	};

	enum class EYAlign
	{
		Null, Top, Bottom, Center
	};

	enum class EDirection
	{
		Null, X, Y, Z
	};

	enum class EOrder
	{
		Null, Up, Down
	};

	UOS_LINKING EYAlign							ToYAlign(const CString & name);
	UOS_LINKING EXAlign							ToXAlign(const CString & name);
	UOS_LINKING EDirection							ToDirection(const CString & name);
}
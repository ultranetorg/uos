#pragma once

namespace uos
{

	enum class ECullMode
	{
		None	=1,
		CW		=2,
		CCW		=3
	};
	

	enum class EClipping
	{
		Inherit=0, No=1, Apply=2
	};

	enum class EClipperSequence
	{
		Inherit, New
	};

	enum class ETransformating
	{
		Null, Inherit, Root
	};

	static wchar_t * ToString(EClipping e)
	{
		static wchar_t * name[] = {L"Inherit", L"No", L"Apply"};
		return name[char(e)];
	}
}

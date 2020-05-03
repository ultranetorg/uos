#pragma once
#include "Screen.h"

namespace uos
{
	enum class EInputClass
	{
		Null, Mouse, Keyboard, TouchScreen
	};

	enum class EInputAction
	{
		Null, 
		Off, On, DoubleClick, // toggle
		Char, // char
		Move, // move
		Rotation // rotate
	};

	enum class EControl
	{
		Null			= 0,
		
		LeftButton		= VK_LBUTTON	,
		RightButton		= VK_RBUTTON	,
		Cancel			= VK_CANCEL		,
		MiddleButton	= VK_MBUTTON	,

		XButton1		= VK_XBUTTON1  ,
		XButton2		= VK_XBUTTON2  ,
			 
		Back			= VK_BACK      ,
		Tab				= VK_TAB       ,
			 
		Clear			= VK_CLEAR     ,
		Return			= VK_RETURN    ,
			 
		Shift			= VK_SHIFT     ,
		Control			= VK_CONTROL   ,
		Menu			= VK_MENU      ,
		Pause			= VK_PAUSE     ,
		Capital			= VK_CAPITAL   ,
			 
		Kana			= VK_KANA      ,
		Hangeul			= VK_HANGEUL   ,
		Hangul			= VK_HANGUL    ,
			 
		Junja			= VK_JUNJA     ,
		Final			= VK_FINAL     ,
		Hanja			= VK_HANJA     ,
		Kanji			= VK_KANJI     ,
			  
		Escape			= VK_ESCAPE    ,
			 
		Convert			= VK_CONVERT   ,
		NonConvert		= VK_NONCONVERT,
		Accept			= VK_ACCEPT    ,
		ModeChange		= VK_MODECHANGE,
			 
		Space			= VK_SPACE     ,
		Prior			= VK_PRIOR     ,
		Next			= VK_NEXT      ,
		End				= VK_END       ,
		Home			= VK_HOME      ,
		Left			= VK_LEFT      ,
		Up				= VK_UP        ,
		Right			= VK_RIGHT     ,
		Down			= VK_DOWN      ,
		Select			= VK_SELECT    ,
		Print			= VK_PRINT     ,
		Execute			= VK_EXECUTE   ,
		Snapshot		= VK_SNAPSHOT  ,
		Insert			= VK_INSERT    ,
		Delete			= VK_DELETE    ,
		Help			= VK_HELP      ,
			 
		LeftWin			= VK_LWIN      ,
		RightWin		= VK_RWIN      ,
		Apps			= VK_APPS      ,
			 
		Numpad0			= VK_NUMPAD0   ,
		Numpad1			= VK_NUMPAD1   ,
		Numpad2			= VK_NUMPAD2   ,
		Numpad3			= VK_NUMPAD3   ,
		Numpad4			= VK_NUMPAD4   ,
		Numpad5			= VK_NUMPAD5   ,
		Numpad6			= VK_NUMPAD6   ,
		Numpad7			= VK_NUMPAD7   ,
		Numpad8			= VK_NUMPAD8   ,
		Numpad9			= VK_NUMPAD9   ,
		Multiply		= VK_MULTIPLY  ,
		Add				= VK_ADD       ,
		Separator		= VK_SEPARATOR ,
		Subtract		= VK_SUBTRACT  ,
		Decimal			= VK_DECIMAL   ,
		Divide			= VK_DIVIDE    ,
		F1				= VK_F1        ,
		F2				= VK_F2        ,
		F3				= VK_F3        ,
		F4				= VK_F4        ,
		F5				= VK_F5        ,
		F6				= VK_F6        ,
		F7				= VK_F7        ,
		F8				= VK_F8        ,
		F9				= VK_F9        ,
		F10				= VK_F10       ,
		F11				= VK_F11       ,
		F12				= VK_F12       ,
		F13				= VK_F13       ,
		F14				= VK_F14       ,
		F15				= VK_F15       ,
		F16				= VK_F16       ,
		F17				= VK_F17       ,
		F18				= VK_F18       ,
		F19				= VK_F19       ,
		F20				= VK_F20       ,
		F21				= VK_F21       ,
		F22				= VK_F22       ,
		F23				= VK_F23       ,
		F24				= VK_F24       ,
			 
		Numlock			= VK_NUMLOCK   ,
		Scroll			= VK_SCROLL    ,
			 
		LeftShift		= VK_LSHIFT    ,
		RightShift		= VK_RSHIFT    ,
		LeftControl		= VK_LCONTROL  ,
		RightControl	= VK_RCONTROL  ,
		LeftMenu		= VK_LMENU     ,
		RightMenu		= VK_RMENU     ,

		Cursor, 
		Wheel, 
		MobileHome,
		Screen
	};
	
	class CInputDevice : public virtual IType
	{
		public:
			UOS_RTTI
			virtual ~CInputDevice(){}
	};

	struct UOS_ENGINE_LINKING CInputMessage
	{
		int								Id;
		EInputClass						Class;
		EControl						Control;
		EInputAction					Action;
		CInputDevice *					Device = null;
		CScreen *						Screen = null;
		CShared *						Values = null;

		static const CInputMessage		Empty;

		CInputMessage(){}
		CInputMessage(CInputMessage const & a);
		CInputMessage(CInputMessage && a);
		~CInputMessage();

		template<typename T> T * ValuesAs()
		{
			return static_cast<T *>(Values);
		}

		template<typename T> T * MakeValues()
		{
			Values = new T();
			*((T *)Values) = {};
			return (T *)Values;
		}
		
		CInputMessage &	operator = (const CInputMessage & a);
	};


	enum class EMoveStage
	{
		Null, Start, End, Move
	};

	struct UOS_ENGINE_LINKING CMouseInput : public CShared
	{
		CFloat2		Position;
		CFloat2		PositionDelta;
		CFloat2		RotationDelta;
	};

	struct UOS_ENGINE_LINKING CKeyboardInput : public CShared
	{
		int64_t		WM = 0;
		int64_t		Flags = 0;
	};

	#undef GetMessage
}
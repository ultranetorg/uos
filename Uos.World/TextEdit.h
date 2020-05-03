#pragma once
#include "Rectangle.h"
#include "Text.h"

namespace uos
{
	class UOS_WORLD_LINKING CTextEdit : public CRectangle, public IIdleWorker
	{
		public:
			using CElement::UpdateLayout;

			CStyle	*									Style;
			CArray<CString>								Lines;
			CFont *										Font;
			CRefList<CVisual *>							Selections;
			//CString										Text;
			CFloat3										Color = CFloat3::Nan;
			bool										Wrap;
			int											YOffset = 0;
			float										Xscroll = 0;
			CVisual *									Caret = null;
			POINT										CaretPos = {0, 0};
			bool										CaretBlinkPhase = false;
			float										CaretTime;
			bool										IsMultiLine = false;
			//CSolidRectangleMesh *						Mesh;

			bool										IsSelecting = false;
			POINT										SelA = {0, 0 };
			POINT										SelB = {0, 0 };
			
			int											TabCount = 4;

			UOS_RTTI
			CTextEdit(CWorldLevel * l, CStyle * s, const CString & name = GetClassName());
			~CTextEdit();

			void										OnKeyboard(CActive *, CActive *, CKeyboardArgs * a);
			void										OnMouse(CActive *, CActive *, CMouseArgs * a);
			void										OnStateChanged(CActive * s, CActive *, CActiveStateArgs * a);
			
			void										Insert(const CString & t);
			void										Select(int ax, int ay, int bx, int by);
			void										UpdateSelection();

			void										SetMultiline(bool e);
			void										SetText(const CString & t);
			void										SetFont(CFont * f);
			void										SetColor(CFloat3 & c);
			CString										GetText();

			int											GetLine(float y);
			int											GetPosition(CString & s, float x);

			bool										AdjustCaret();
			void										UpdateCaret();

			void										Draw() override;

			virtual void DoIdle() override;
	};
}

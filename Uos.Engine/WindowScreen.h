#pragma once
#include "Screen.h"

namespace uos
{
	class CWindowScreen : public CNativeWindow, public CScreen
	{
		public:
			CEvent<MSG &>								MessageRecieved;

			virtual void								Show(bool e) override;
			virtual void								SetCursor(HCURSOR c) override;

			UOS_RTTI
			CWindowScreen(CEngineLevel * l, DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
			~CWindowScreen();


		private:
			bool				 						ProcessMessage(HWND hwnd, UINT msg, WPARAM w, LPARAM l, INT_PTR * r);
	};
}

#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "TouchScreen.h"

namespace uos
{
	class UOS_ENGINE_LINKING CInputSystem : public CEngineEntity
	{
		public:
			CList<CInputDevice *>						Devices;
			CScreenEngine *								ScreenEngine;
			CEvent<CInputMessage &>						Recieved;
			bool										InSending = false;
			CList<CInputMessage>						Deferred;

			UOS_RTTI
			CInputSystem(CEngineLevel * ilw, CScreenEngine * se);
			~CInputSystem();
		
			int											GetNextID() { return MessageCounter++; }
			bool										ProcessMessage(MSG * msg);
			void										SendInput(CInputMessage & m);

			template<class T> T * First()
			{
				for(auto i : Devices)
				{
					if(auto d = i->As<T>())
					{
						return d;
					}
				}

				return null;
			}

		private:
			int											MessageCounter = 0;

	};
}

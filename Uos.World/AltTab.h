/*
#pragma once
#include "INavigator.h"
#include "WorldNode.h"

namespace Mightywill
{
	class CActor;
	
	class CAltTab : public CEntity3World, public IHotKeyHandler, public INavigator
	{
		public:
			bool										IsActive;
			
			void										Update(bool camera, bool layout);
			bool										CanActivate();
			void										Activate(CEye * dp);
			void										Deactivate();
			bool										IsTotal();

			void 										CreateOptions(IOptioner * o, IOptionLocation * root);
			void										DeleteOptions();
			
			MW_DEFINE_RTTI
			CAltTab(CActor * e, IParameter * p);
			~CAltTab();
		
		private:
			float										AnimateTime;
			float										Curvature;
			float										XSpacing;
			float										YSpacing;
			float										SelectionAreaMinWidth;
			float										SelectionScale;
			
			IParameter *								Parameter;
			CActor *									Actor;
			IGodeNode *										TitleParent;
			
			IOptioner *									Optioner;
			IOptionBlock *								OptionBlock;
			IOptionLocation *							OptionLocation;
	
			CEye *										Eye;
			CArray<CWorldNode *>						Nodes;
			int											AltTabHotKeyIdNext;
			int											AltTabHotKeyIdPrev;
			int											Index;

			void										ProcessHotKey(int id);
			void										Build();
			bool static									SortOperator(CWorldNode * l, CWorldNode * r);
			void 										OnOptionBlockInited();
			void										OnOptionBlockCommited();
			void 										LoadParameteres();
			void 										OnObjectAddedRemoved(CWorldNode *);
			bool 										TryCollectObjects();
	};*/
}
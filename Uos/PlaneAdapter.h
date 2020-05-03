#pragma once
#include "IPlaneArranger.h"

//#include "WorldNode.h"

namespace uos
{
	class CPlaneAdapter : public IAdapter
	{
		public:
			static const wchar_t *						WORLDNODE_METANAME;

			float										Distance;
			float										X;
			float										Y;
			//CWorldNode *								WNode;
			bool										Manual;
			bool										Arranged;
			IPlaneArranger *							Arranger;
														
			CTransformation								GetNormalTransformation();
			void										SetNormalPosition(bool custom, float d, float x, float y);
			bool										IsReal();
			bool										IsManual();

			CPlaneAdapter(IPlaneArranger * a);
			~CPlaneAdapter();
		
		private:
	};
}
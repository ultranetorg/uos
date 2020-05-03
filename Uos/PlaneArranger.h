#pragma once
#include "IPlaneArranger.h"
#include "PlaneAdapter.h"
#include "Arranger.h"
#include "Plane.h"
#include "Rect.h"

namespace uos
{
	enum EClause
	{
		EClause_True, EClause_False, EClause_Any
	};

	class CPlaneArranger : public CArranger, public IPlaneArranger
	{
		public:
			void										ArrangeDefault(CArray<IArrangable *> & wns, CAnimation & ani, float w);
			void										ArrangeSortingV(CArray<IArrangable *> & wns, CAnimation & ani);
			void										ArrangeSortingH(CArray<IArrangable *> & wns, CAnimation & ani);
			void										ArrangeSortingR(CArray<IArrangable *> & wns, CAnimation & ani);

			void										Place(CArray<IArrangable *> & wns, CSize & s, IArrangable * wn, CAnimation & ani);

			//void										CreateOptions(IOptioner * o, IOptionLocation * pl);
			//void										DeleteOptions();

			CPlaneArranger();
			~CPlaneArranger();
		
		private:
			float										CapturedDist;
			float										CapturedX;
			float										CapturedY;
			
			CPlane										Plane;
			float										NormalDistance;

			/*IParameter *								RootParameter;
			IOptioner *									Optioner;
			IOptionBlock *								OptionBlock;
			IOptionLocation *							OptionLocation;*/
			
			CArray<IArrangable *>						WaitingNodes;
			
			//void										ProcessNodeAxisEvent(IArrangable * wn, CCursorMoveEventArgs & a);
			//void										ProcessNodeMouseButtonEvent(IArrangable * wn, CCursorEventArgs & a);

			void										ArrangeHorizontally(CArray<CSortKey> & keys, float h, float spacingX, int start, int end);
			void										ArrangeVertically(CArray<CSortKey> & keys, float yaw, int start, int end);
			void										ArrangeByDistance(CArray<CSortKey> & keys, float yaw, int start, int end);
			
			CArray<IArrangable *>						FetchArrangables(CArray<IArrangable *> & nodes, EClause hasAdapter, EClause adIsActive, EClause adIsReal, EClause trasformApplied);

			void										OnOptionBlockInited();
			void										OnOptionBlockCommited();
			void										ApplyParameters();
			
			void										PlaceCentripetally3(CArray<IArrangable *> & wns, IArrangable * wn, CAnimation & ani);
			bool										TryPlace(CArray<CRect> & wns, CRect & r);
			bool										RectsContainPoint(CArray<CRect> & rects, float x, float y);
			bool										RectsContainPointWOB(CArray<CRect> & rects, float x, float y);
			CArray<CRect>								GetInflatedRects(CArray<IArrangable *> & wns, IArrangable * wn, float spacing);
			
			CArray<CSortKey>							PrepareArrange(CArray<IArrangable *> & wns);
			void										FinilizeArrange(CArray<IArrangable *> & wns, CAnimation & ani);
	};
}
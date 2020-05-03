#pragma once
#include "IArranger.h"

namespace uos
{
	class IPlaneArranger : public IArranger
	{
		public:
			virtual void								ArrangeDefault(CArray<IArrangable *> & wns, CAnimation & ani, float w)=0;
			virtual void								ArrangeSortingV(CArray<IArrangable *> & wns, CAnimation & ani)=0;
			virtual void								ArrangeSortingH(CArray<IArrangable *> & wns, CAnimation & ani)=0;
			virtual void								ArrangeSortingR(CArray<IArrangable *> & wns, CAnimation & ani)=0;

			virtual void								Place(CArray<IArrangable *> & wns, CSize & s, IArrangable * wn, CAnimation & ani)=0;

			//virtual void								Arrange(EArrangement a, CArea & ar)=0;
			//virtual void								SetArrangement(EArrangement a, CSite * s)=0;

			virtual ~IPlaneArranger(){}
	};
}
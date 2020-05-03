#pragma once
#include "Float3.h"
#include "Matrix.h"
#include "Size.h"

namespace uos
{
/*
	template<typename T, typename C> class Property
	{
		public:
			using GetterType = T(C::*)();

			Property(C* theObject, GetterType theGetter)
				:itsObject(theObject),
				itsGetter(theGetter)
			{ }

			operator T()
			{
				return (itsObject->*itsGetter)();
			}

			private:
			C* itsObject;
			GetterType itsGetter;
	};*/

	class UOS_LINKING CAABB
	{
		public:
			CFloat3										Min;
			CFloat3										Max;

			static const CAABB							Empty;
			static const CAABB							InversedMax;

			//Property<float, CAABB>						Width{this,&CAABB::GetWidth};

			CAABB();
			CAABB(float w, float h, float d);
			CAABB(CFloat3 a, CFloat3 b);
			CAABB(float minx, float miny, float minz, float maxx, float maxy, float maxz);
			~CAABB();


			float										GetWidth();
			float										GetHeight();
			float										GetDepth();

			CAABB										Join(CAABB & a);
			CAABB										Cross(CAABB & a);
			bool										Contains(CFloat3 & p);
			bool										Contains(CFloat3 & p, float eps);
			void										Join2D(CTransformation & t, CSize & a);
			CSize										GetSize();
			CAABB										Transform(CMatrix const & m);
			CString										ToNiceString();

			template<class T> static CSize				Calculate(T & items)
														{
															auto bb = CAABB::InversedMax;

															for(auto i : items)
															{
																bb.Join2D(i->Transformation, i->Size);
															}
															return bb.GetSize();
														}
	};
}

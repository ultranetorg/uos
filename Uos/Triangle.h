#pragma once
#include "Float3.h"

namespace uos
{
	struct CTriIntersection
	{
		float		Distance;
		CFloat3		Point,Vector;
		bool		Return;
		float		U;
		float		V;
	};

	struct TSpher
	{
		CFloat3	pos;//!!локальная позиция!! (Сдесь она не используется)
		float		r; //радиус сферы
	};


	class UOS_LINKING CTriangle
	{
		public:
			CFloat3	Normals[4]; //нормали треугла.... расчитываются один раз в Creat'е
			CFloat3	pos,center; //локальная позиция треугла, Center-мировая позиция треугла
			CFloat3	Vertices[3]; //вершины треугла

			CTriangle();
			~CTriangle();
			
			static CTriIntersection 					LegIntersection(CFloat3 & v1, CFloat3 & v2, CFloat3 & v3, CFloat3 & begin, CFloat3 & end);
			static CTriIntersection 					SphereIntersection(CTriangle & treagle, TSpher & Spher, CFloat3 & posSpher);
			static bool									Intersect(CFloat3 & a, CFloat3 & b, CFloat3 & c, CRay & ray, bool testCull, CTriIntersection * is);
			static bool									ContainsPoint(CFloat2 & a, CFloat2 & b, CFloat2 & c, CFloat2 & p);
	};
}
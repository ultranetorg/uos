#pragma once
#include "Line.h"

namespace uos
{
	class CCircle
	{
		public:
			CFloat2	C;
			float	R;

			CCircle(CFloat2 & c, float r)
			{
				C = c;
				R = r;
			}

			int Intersect(CLine & l, CFloat2 * p1, CFloat2 * p2, float lt)
			{
				// проекция центра окружности на прямую
				auto p = l.ClosestPoint(C);
				// сколько всего решений?
				int flag = 0;

				auto d = C.GetDistance(p);

				if(abs(d - R) <= lt)
					flag = 1;
				else
					if(R > d)
						flag = 2;
					else
						return 0;

				// находим расстояние от проекции до точек пересечения
				auto k = sqrt(R * R - d * d);
				auto t = CFloat2(l.B, -l.A).GetLength();
				// добавляем к проекции векторы направленные к точкам пеерсечения
				*p1 = add_vector(p, CFloat2(0, 0), CFloat2(-l.B, l.A), k / t);
				*p2 = add_vector(p, CFloat2(0, 0), CFloat2(l.B, -l.A), k / t);

				return flag;
			}

			int Intersect(CFloat2 & ra, CFloat2 & rb, CFloat2 * p1, CFloat2 * p2, float lt) // lt = line thickness
			{
				// пересекаем прямую луча с окружностью
				auto l = CLine(ra, rb);
				int flag = Intersect(l, p1, p2, lt);

				if(flag == 0)
					return 0;

				// если точки пересечения есть, то проверяем их на принадлежность лучу
				// если точка одна
				if(flag == 1)
					if(point_in_ray(*p1, ra, rb, lt))
						return 1;
					else
						return 0;

				// если точки две
				bool b1 = point_in_ray(*p1, ra, rb, lt);
				bool b2 = point_in_ray(*p2, ra, rb, lt);

				if(b1)
					if(b2)
						return 2;
					else
						return 1;
				else
					if(b2)
					{
						*p1 = *p2;
						return 1;
					}
					else
						return 0;
			}

			CFloat2 add_vector(CFloat2 & p, CFloat2 & p1, CFloat2 & p2, float k)
			{
				return CFloat2(p.x + (p2.x - p1.x) * k, p.y + (p2.y - p1.y) * k);
			}

			bool point_in_ray(CFloat2 & p, CFloat2 & r1, CFloat2 & r2, float thickness)
			{
				// принадлежит ли точка прямой луча
				auto l = CLine(r1, r2);
				///if(l.point_in_line(p) != 0)
				if(l.Distance(p) > thickness)
					return false;

				// если прямая вертикальная, то проверяем на y
				if(abs(l.B) <= FLT_EPSILON)
					if(r2.y >= r1.y)
						return p.y >= r1.y;
					else
						return p.y <= r1.y;

				// иначе проверяем на x 
				if(r2.x >= r1.x)
					return p.x >= r1.x;
				else
					return p.x <= r1.x;
			}


			//			bool Intersect(float r, CFloat2 & p, CFloat2 & d, CFloat2 * ia, CFloat2 * ib) // p = origin, d = direction of a line
			//			{
			//				auto a = -d.y;
			//				auto b = d.x;
			//				auto c = -a * p.x - b * p.y;
			//					
			//				///float r, a, b, c; // входные данные
			//
			//				auto x0 = -a*c/(a*a+b*b),  y0 = -b*c/(a*a+b*b);
			//				
			//				if(c*c > r*r*(a*a+b*b)+EPS)
			//				{
			//					return false;
			//				}
			//				else if (abs (c*c - r*r*(a*a+b*b)) < EPS)
			//				{
			//					ia->x = x0;
			//					ia->y = y0;
			//				}
			//				else
			//				{
			//					auto d = r*r - c*c/(a*a+b*b);
			//					auto mult = sqrt (d / (a*a+b*b));
			//					auto ax,ay,bx,by;
			//					ax = x0 + b * mult;
			//					bx = x0 - b * mult;
			//					ay = y0 - a * mult;
			//					by = y0 + a * mult;
			//
			//					ia->x = ax;
			//					ia->y = ay;
			//
			//					ib->x = bx;
			//					ib->y = by;
			//				}
			//			}*/

	};
}

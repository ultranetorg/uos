#pragma once
#include "Float2.h"

namespace uos
{
	class CLine
	{
		public:
			float A, B, C;

			// Создание прямой ax + by + c = 0
			CLine(float _a = 0, float _b = 0, float _c = 0)
			{
				A = _a;
				B = _b;
				C = _c;
			}

			CLine(CFloat2 & p1, CFloat2 & p2)
			{
				A = p2.y - p1.y;
				B = p1.x - p2.x;
				C = -A*p1.x - B*p1.y;
			}

			float GetY(float x)
			{
				return (-C - A*x)/B;
			}

			float Distance(CFloat2 & p)
			{
				return abs((A * p.x + B * p.y + C) / sqrt(A * A + B * B));
			}
			
			// проекция точки на прямую
			CFloat2 ClosestPoint(CFloat2 & p)
			{
				auto k = (A * p.x + B * p.y + C) / (A * A + B * B);
				return CFloat2(p.x - A * k, p.y - B * k);
			}

//			int point_in_line (CFloat2 & p)
//			{
//				auto s = A * p.x + B * p.y + C;
//				return s < -1.f ? -1 : s > 1.f ? 1 : 0;
//			}


	};

	class CLine3
	{
		public:
/*
			float A, B, C, D;

			// Создание прямой ax + by + c = 0
			CLine(float a, float b, float c, float d)
			{
				A = a;
				B = b;
				C = c;
				D = d;
			}

			CLine(CFloat3 & p1, CFloat3 & p2)
			{
				A = p2.y - p1.y;
				B = p1.x - p2.x;
				C = -A*p1.x - B*p1.y;
			}

			float GetY(float x, float z)
			{
				return (-D - A*x - C*z)/B;
			}*/
	};

}

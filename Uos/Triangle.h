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
		CFloat3	pos;//!!��������� �������!! (����� ��� �� ������������)
		float		r; //������ �����
	};


	class UOS_LINKING CTriangle
	{
		public:
			CFloat3	Normals[4]; //������� �������.... ������������� ���� ��� � Creat'�
			CFloat3	pos,center; //��������� ������� �������, Center-������� ������� �������
			CFloat3	Vertices[3]; //������� �������

			CTriangle();
			~CTriangle();
			
			static CTriIntersection 					LegIntersection(CFloat3 & v1, CFloat3 & v2, CFloat3 & v3, CFloat3 & begin, CFloat3 & end);
			static CTriIntersection 					SphereIntersection(CTriangle & treagle, TSpher & Spher, CFloat3 & posSpher);
			static bool									Intersect(CFloat3 & a, CFloat3 & b, CFloat3 & c, CRay & ray, bool testCull, CTriIntersection * is);
			static bool									ContainsPoint(CFloat2 & a, CFloat2 & b, CFloat2 & c, CFloat2 & p);
	};
}
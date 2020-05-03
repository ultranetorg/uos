#pragma once
#include "Mesh.h"

namespace uos
{
	class UOS_ENGINE_LINKING CFrameMesh : public CMesh
	{
		public:
			UOS_RTTI
			CFrameMesh(CEngineLevel * l) : CMesh(l)
			{
				SetVertices(UOS_MESH_ELEMENT_POSITION, CArray<CFloat3>());

				CArray<int> i;
				i.reserve(48);

				i.push_back(8);	 i.push_back(9);	i.push_back(4); i.push_back(4); i.push_back(9);	 i.push_back(5); //left
				i.push_back(10); i.push_back(11);	i.push_back(6); i.push_back(6); i.push_back(11); i.push_back(7); //right
				i.push_back(5);	 i.push_back(6);	i.push_back(1); i.push_back(1); i.push_back(6);	 i.push_back(2); //bottom
				i.push_back(13); i.push_back(14);	i.push_back(9); i.push_back(9); i.push_back(14); i.push_back(10); //top

				i.push_back(4);	 i.push_back(5);	i.push_back(0); i.push_back(0); i.push_back(5);	 i.push_back(1); //b-l
				i.push_back(6);	 i.push_back(7);	i.push_back(2); i.push_back(2); i.push_back(7);	 i.push_back(3); //b-r
				i.push_back(12); i.push_back(13);	i.push_back(8); i.push_back(8); i.push_back(13); i.push_back(9); //t-l
				i.push_back(14); i.push_back(15);	i.push_back(10);i.push_back(10);i.push_back(15); i.push_back(11); //t-r
		
				SetIndexes(i);
				SetPrimitiveInfo(EPrimitiveType::TriangleList);	
			}

			void Generate(float x, float y, float z, float iw, float ih, float l, float b, float r, float t)
			{
				CArray<CFloat3> p;

				p.push_back(CFloat3(x,					y, z));
				p.push_back(CFloat3(x + l,				y, z));
				p.push_back(CFloat3(x + l + iw,			y, z));
				p.push_back(CFloat3(x + l + iw + r,		y, z));
																
				p.push_back(CFloat3(x,					y + b, z));
				p.push_back(CFloat3(x + l,				y + b, z));
				p.push_back(CFloat3(x + l + iw,			y + b, z));
				p.push_back(CFloat3(x + l + iw + r,		y + b, z));
																
				p.push_back(CFloat3(x,					y + b + ih, z));
				p.push_back(CFloat3(x + l,				y + b + ih, z));
				p.push_back(CFloat3(x + l + iw,			y + b + ih, z));
				p.push_back(CFloat3(x + l + iw + r,		y + b + ih, z));
																
				p.push_back(CFloat3(x,					y + b + ih + t, z));
				p.push_back(CFloat3(x + l,				y + b + ih + t, z));
				p.push_back(CFloat3(x + l + iw,			y + b + ih + t, z));
				p.push_back(CFloat3(x + l + iw + r,		y + b + ih + t, z));

				SetVertices(UOS_MESH_ELEMENT_POSITION, p);
				SetBBox(CAABB(x, y, 0, x + l + iw + r, y + b + ih + t, z));
			}
	};
}


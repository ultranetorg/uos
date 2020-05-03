#pragma once
#include "Mesh.h"

namespace uos
{
	class UOS_ENGINE_LINKING CBoxMesh : public CMesh
	{
		public:
			void Generate(float x, float y, float z, float w, float h, float d)
			{
				CArray<CFloat3> p;
				CArray<int> i ={0,1,2,0,2,3,
								4,5,6,4,6,7,
								8,9,10,8,10,11,
								12,13,14,12,14,15,
								16,17,18,16,18,19,
								20,21,22,20,22,23};
				// front
				p.push_back(CFloat3(x, y, z));
				p.push_back(CFloat3(x, y+h, z));
				p.push_back(CFloat3(x+w, y+h, z));
				p.push_back(CFloat3(x+w, y, z));
    			 // back			
				p.push_back(CFloat3(x, y, z+d));
				p.push_back(CFloat3(x+w, y, z+d));
				p.push_back(CFloat3(x+w, y+h, z+d));
				p.push_back(CFloat3(x, y+h, z+d));
    					 // top	
				p.push_back(CFloat3(x, y+h, z));
				p.push_back(CFloat3(x, y+h, z+d));
				p.push_back(CFloat3(x+w, y+h, z+d));
				p.push_back(CFloat3(x+w, y+h, z));
    			 // bottom				
				p.push_back(CFloat3(x, y, z));
				p.push_back(CFloat3(x+w, y, z));
				p.push_back(CFloat3(x+w, y, z+d));
				p.push_back(CFloat3(x, y, z+d));
    			// left			
				p.push_back(CFloat3(x, y, z+d));
				p.push_back(CFloat3(x, y+h, z+d));
				p.push_back(CFloat3(x, y+h, z));
				p.push_back(CFloat3(x, y, z));
    			// right		
				p.push_back(CFloat3(x+w, y, z));
				p.push_back(CFloat3(x+w, y+h, z));
				p.push_back(CFloat3(x+w, y+h, z+d));
				p.push_back(CFloat3(x+w, y, z+d));

				SetVertices(UOS_MESH_ELEMENT_POSITION, p);
				SetIndexes(i);
				SetPrimitiveInfo(EPrimitiveType::TriangleList);	

				BBox = CAABB(CFloat3(x, y, z), CFloat3(x + w, y + h, z + d));
			}
			
			UOS_RTTI
			CBoxMesh(CEngineLevel * l) : CMesh(l)
			{
				SetVertices(UOS_MESH_ELEMENT_POSITION, CArray<CFloat3>());
			}
	};
}

#pragma once
#include "Mesh.h"

namespace uos
{
	class UOS_ENGINE_LINKING CSphereMesh : public CMesh
	{
		public:
			UOS_RTTI
			CSphereMesh(CEngineLevel * l) : CMesh(l)
			{
				SetPrimitiveInfo(EPrimitiveType::TriangleList);	
			}

			void Generate(float x, float y, float z, float r, int n)
			{
				auto m = n + 1;

				CArray<CFloat3> points;
				CArray<CFloat2> uv;
				CArray<int>		ix;

				points.resize(m * m);
				uv.resize(m * m);


				for(int lt=0; lt < m; lt++)
				{
					for(int lg=0; lg < m; lg++)
					{
						CFloat3 p;

						points[lt * m + lg].x = x + float(r * sin(lt) * cos(lg));
						points[lt * m + lg].z = z + float(r * sin(lt) * sin(lg));
						points[lt * m + lg].y = y + float(r * cos(lt));

						uv[lt * m + lg].x = float(lg) / n;
						uv[lt * m + lg].y = float(lt) / n;
					}
				}


				for(int lt = 0; lt < n; lt++)
				{
					for(int lg = 0; lg < n; lg++)
					{
						ix.push_back(lt * m + lg);
						ix.push_back(lt * m + lg + 1);
						ix.push_back((lt + 1) * m + lg);

						ix.push_back((lt + 1) * m + lg);
						ix.push_back(lt * m + lg + 1);
						ix.push_back((lt + 1) * m + lg + 1);
					}
				}

				SetVertices(UOS_MESH_ELEMENT_POSITION, points);
				SetVertices(UOS_MESH_ELEMENT_UV, uv);
				SetIndexes(ix);

				BBox = CAABB(CFloat3(-r, -r, -r), CFloat3(r, r, r));
			}
	};
}


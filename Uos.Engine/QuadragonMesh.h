#pragma once
#include "Mesh.h"

namespace uos
{
	class UOS_ENGINE_LINKING CSolidQuadragonMesh : public CMesh
	{
		public:
			UOS_RTTI
			CSolidQuadragonMesh(CEngineLevel * l, bool flipuv = false) : CMesh(l)
			{
				SetVertices(UOS_MESH_ELEMENT_POSITION, CArray<CFloat3>());
				GenerateUV(flipuv);
				SetIndexes(CArray<int>{0, 1, 2, 0, 2, 3});
				SetPrimitiveInfo(EPrimitiveType::TriangleList);	
			}

			void GenerateUV(bool flipuv)
			{
				CArray<CFloat2> uv(4);

				if(!flipuv)
				{
					uv[1] = CFloat2(0.f, 0.f);	uv[2] = CFloat2(1.f, 0.f);
					uv[0] = CFloat2(0.f, 1.f);	uv[3] = CFloat2(1.f, 1.f);
				} 
				else
				{
					uv[1] = CFloat2(0.f, 1.f);	uv[2] = CFloat2(1.f, 1.f);
					uv[0] = CFloat2(0.f, 0.f);	uv[3] = CFloat2(1.f, 0.f);
				}
			
				SetVertices(UOS_MESH_ELEMENT_UV, uv);
			}

			void Generate(CFloat2 const & a, CFloat2 const & b, CFloat2 const & c, CFloat2 const & d)
			{
				SetVertices(UOS_MESH_ELEMENT_POSITION, CArray<CFloat3>{{a.x, a.y, 0}, {b.x, b.y, 0}, {c.x, c.y, 0}, {d.x, d.y, 0}});
			}
	};

	class UOS_ENGINE_LINKING CSolidRectangleMesh : public CSolidQuadragonMesh
	{
		public:
			UOS_RTTI
			float X = NAN;
			float Y = NAN;
			float W = NAN;
			float H = NAN;

			CSolidRectangleMesh(CEngineLevel * l, bool flipuv = false) : CSolidQuadragonMesh(l, flipuv)
			{
			}

			using CSolidQuadragonMesh::Generate;

			void Generate(float x, float y, float w, float h)
			{
				if(x != X || y != Y || w != W || h != H)
				{
					X = x;
					Y = y;
					W = w;
					H = h;

					CFloat2 a, b, c, d;
	
					b.x = x;			c.x = x + w;
					b.y = y + h;		c.y = y + h;
	
					a.x = x;			d.x = x + w;
					a.y = y;			d.y = y;
	
					Generate(a, b, c, d);
					
					BBox = CAABB(CFloat3(x, y, 0), CFloat3(x + w, y + h, 0));
				}
			}
	};

	class UOS_ENGINE_LINKING CGridQuadragonMesh : public CMesh
	{
		public:
			UOS_RTTI
			CGridQuadragonMesh(CEngineLevel * l) : CMesh(l)
			{
				SetVertices(UOS_MESH_ELEMENT_POSITION, CArray<CFloat3>());
				SetPrimitiveInfo(EPrimitiveType::LineList);
			}

			void Generate(CFloat2 const & a, CFloat2 const & b, CFloat2 const & c, CFloat2 const & d, int nx, int ny, float t)
			{
				CArray<CFloat3> vx;
				CArray<int>	ix;

				for(int i=0; i<=nx; i++)
				{
					vx.push_back(CFloat3(a.x + (d.x - a.x) * i/nx, a.y + (d.y - a.y) * i/nx, 0));
					vx.push_back(CFloat3(b.x + (c.x - b.x) * i/nx, b.y + (c.y - b.y) * i/nx, 0));

					ix.push_back((int)ix.size());
					ix.push_back((int)ix.size());
				}

				for(int i=0; i<=ny; i++)
				{
					vx.push_back(CFloat3(a.x + (b.x - a.x) * i/ny, a.y + (b.y - a.y) * i/ny, 0));
					vx.push_back(CFloat3(d.x + (c.x - d.x) * i/ny, d.y + (c.y - d.y) * i/ny, 0));

					ix.push_back((int)ix.size());
					ix.push_back((int)ix.size());
				}

				SetVertices(UOS_MESH_ELEMENT_POSITION, vx);
				SetIndexes(ix);
			}
	};

	class UOS_ENGINE_LINKING CGridRectangleMesh : public CGridQuadragonMesh
	{
		public:
			UOS_RTTI
			CGridRectangleMesh(CEngineLevel * l) : CGridQuadragonMesh(l)
			{
			}

			using CGridQuadragonMesh::Generate;

			void Generate(float x, float y, float z, float w, float h, int nx, int ny, float t)
			{
				CArray<CFloat3> vx;
				CArray<int>	ix;

				w -= t;
				h -= t;

				for(int i=0; i <= nx; i++)
				{
					vx.push_back(CFloat3(x + i * w/nx, y,	  z));
					vx.push_back(CFloat3(x + i * w/nx, y + h, z));

					ix.push_back((int)ix.size());
					ix.push_back((int)ix.size());
				}

				for(int i=0; i <= ny; i++)
				{
					vx.push_back(CFloat3(x,		t + y + i * h/ny, z));
					vx.push_back(CFloat3(x + w, t + y + i * h/ny, z));

					ix.push_back((int)ix.size());
					ix.push_back((int)ix.size());
				}

				SetVertices(UOS_MESH_ELEMENT_POSITION, vx);
				SetIndexes(ix);
			}

	};
}

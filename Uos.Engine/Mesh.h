#pragma once
#include "EngineLevel.h"
#include "Vertexes.h"

namespace uos
{
	class UOS_ENGINE_LINKING CMeshIntersection
	{
		public:
			float				Distance = FLT_MAX;
			int					StartIndex = -1;
			CFloat3				Point = CFloat3::Nan;

			bool operator != (const CMeshIntersection & a)
			{
				return Point != Point || Distance != a.Distance || StartIndex != a.StartIndex;
			}
	};

	enum class EPrimitiveType
	{
		Null = 0,
		PointList = 1,
		LineList = 2,
		LineStrip = 3,
		TriangleList = 4,
		TriangleStrip = 5,
		TriangleFan = 6
	};

	enum class EPrimitiveFill
	{
		Null = 0,
		Point = 1,
		Wireframe = 2,
		Solid = 3
	};

	class CMeshRealization;


	class UOS_ENGINE_LINKING CMesh : public CEngineEntity, public CShared
	{
		public:
			EPrimitiveType								PrimitiveType = EPrimitiveType::TriangleList;
			EPrimitiveFill								FillMode = EPrimitiveFill::Solid;

			CAABB										BBox = CAABB::Empty;
			
			CArray<CVertexes *>							Vertexes;
			CArray<int>									Indexes;
			
			CMeshRealization *							Realization;

			UOS_RTTI
			CMesh(CEngineLevel * l);
			virtual ~CMesh();

			bool										IsReady();

			void										SetPrimitiveInfo(EPrimitiveType pt);
			void										SetFillMode(EPrimitiveFill f);

			void										SetBBox(CAABB bb);
			CAABB &										GetBBox();
		
			void										AddVertices(const CString & name, CString const & type, short dim, void * data);
			void										SetVertices(const CString & name, CArray<CFloat2> & v);
			void										SetVertices(const CString & name, CArray<CFloat3> & v);
			void										SetVertices(const CString & name, CArray<int> & v);
			void										SetIndexes(CArray<int> & i);

			void										Save(CXon *);
			void										Load(CXon * p);

			CFloat3										GetIntersectionPoint(CTriIntersection & is, int start);
			CMeshIntersection							Intersect(CRay & r);

			void										Clear();
			void										Merge(CMesh * m);


			template<class T> CArray<T> &				GetVertexes(CString const & name)
														{
															auto v = Vertexes.Find([&name](auto & i){ return i->Name == name; });
															if(!v)
															{
																throw CException(HERE, L"Vertex element not found");
															}

															return *((CArray<T> *)v->Data);
														}
	};
}



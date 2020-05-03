#pragma once
#include "DirectSystem.h"
#include "Mesh.h"

namespace uos
{
	class CMeshRealization
	{
		public:
			bool										IsFormatModified = false;
			virtual void								Invalidate(bool format, bool vertexes, bool indexes)=0;

			virtual ~CMeshRealization(){}
	};

	class CDirectMeshBuffer
	{
		public:
			CDirectDevice *								Device = null;
			bool										IsBuilt = false;

			ID3D11Buffer *								VB = null;
			ID3D11Buffer *								IB = null;
			D3D11_BUFFER_DESC							VbDesc = {};
			D3D11_BUFFER_DESC							IbDesc = {};

			bool										IsVertexesChanged = true;
			bool										IsFormatChanged = true;
			bool										IsIndexesChanged = true;

			CDirectMeshBuffer()
			{
			}

			CDirectMeshBuffer(CDirectDevice * gd)
			{
				Device = gd;
			}

			~CDirectMeshBuffer()
			{
				if(VB != null)
				{
					VB->Release();
				}
				if(IB != null)
				{
					IB->Release();
				}
			}

			int GetIndexSize()
			{
				return 4;
			}

			void										UpdateIndexes(CArray<int> & ix);
			void										UpdateVertexes(CArray<CVertexes *> & data);
	};

	class UOS_ENGINE_LINKING CDirectMesh : public CEngineEntity, public CMeshRealization
	{
		public:
			CMap<CDirectDevice *, CDirectMeshBuffer *>	Buffers;
			CMesh *										Mesh;
			int											VertexSize;
			D3D11_PRIMITIVE_TOPOLOGY					Topology;

			UOS_RTTI
			CDirectMesh(CEngineLevel * l, CMesh * m);
			virtual ~CDirectMesh();

			void										Draw(CDirectDevice * gd);
			void										DrawRepeat(CDirectDevice * gd);

			///void										BuildShaderCode(CShader * c) override;
			void										Invalidate(bool format, bool vertexes, bool indexes) override;
	};


	static D3D11_PRIMITIVE_TOPOLOGY ToDx(EPrimitiveType m)
	{
		switch(m)
		{
			case EPrimitiveType::Null:				return  D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
			case EPrimitiveType::PointList:			return  D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
			case EPrimitiveType::LineList:			return  D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
			case EPrimitiveType::LineStrip:			return  D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
			case EPrimitiveType::TriangleList:		return  D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case EPrimitiveType::TriangleStrip:		return  D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		}
		
		throw CException(HERE, L"Not supported");
	}

	static D3D11_FILL_MODE ToDx(EPrimitiveFill m)
	{
		switch(m)
		{
			case EPrimitiveFill::Wireframe:		return  D3D11_FILL_WIREFRAME;
			case EPrimitiveFill::Solid:			return  D3D11_FILL_SOLID;
		}

		throw CException(HERE, L"Not supported");
	}

}
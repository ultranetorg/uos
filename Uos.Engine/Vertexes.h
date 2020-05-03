#pragma once

#define UOS_MESH_ELEMENT_POSITION		L"Position"
#define UOS_MESH_ELEMENT_NORMAL			L"Normal"
#define UOS_MESH_ELEMENT_UV				L"UV"
#define UOS_MESH_ELEMENT_VERTEXCOLOR	L"Diffuse"

namespace uos
{
	class CVertexes
	{
		public:
			CString										Name;
			CString										Type;
			short										Dim;
			void *										Data = null;

			int											GetVertexSize();
			int											GetCount();
			int											GetSize();
			void										Insert(void * vertexes, int offset = -1);

			~CVertexes();
	};
}
#pragma once
#include "MaterialPool.h"

namespace uos
{
	#define  UOS_STYLE_TEXT		L"Text"
	#define  UOS_STYLE_BORDER	L"Border"

	#define  UOS_PROP_FONT		L"Font"
	#define  UOS_PROP_COLOR		L"Color"
	#define  UOS_PROP_MATERIAL	L"Material"

	class UOS_WORLD_LINKING CStyle
	{
		public:
			CTonDocument *								Document;	
			CEngine *									Engine;
			CMaterialPool *								Pool;
			CMap<CString, CMesh *>						Meshes;

			CStyle(CEngine * e, CMaterialPool * p);
			CStyle(CEngine * e, CMaterialPool * p, IXonReader & r);
			~CStyle();

			template<class T> T Get(CString const & name)
			{
				auto p = Document->One(name);

				T t;
				t.Read(p->AsString());
				return t;
			}

			CMaterial *									GetMaterial(CString const & name);
			CFont *										GetFont(CString const & name);
			CAnimation									GetAnimation(CString const & name);
			CMesh *										GetMesh(CString const & name);

			void										DefineMesh(CString const & name, CMesh * mesh);

			CStyle * Clone();
	};
}


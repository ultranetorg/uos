#pragma once
#include "WorldLevel.h"
#include "Element.h"

namespace uos
{
	class UOS_WORLD_LINKING CVwmImporter : public virtual IType//, public IModelImporter
	{
		public:
			void										Import(IDirectory * dir);
			CXonDocument *								GetMetadata();

			//CVisual	*									LoadTree(CXon * p);
			CElement *								ImportNodeTree(CXon * p);
			void										ImportVisual(CXon * n, CXon * vn, CVisual * v);
			void										ImportActive(CXon * n, CXon * vn, CActive * v);
			CMesh *										ImportMesh(const CString & file);
			CMaterial *									ImportMaterial(const CString & file, CLight * l);

			UOS_RTTI
			CVwmImporter(CWorldLevel * l);
			~CVwmImporter();

			CWorldLevel *								Level;
			CEngine *									Engine;
			CXonDocument *								Graph;
			IDirectory *								Directory;

//			CTreeMap<IParameter *, CVisual *>			Visuals;
			CMap<CString, CMesh *>						Meshes;
			CMap<CString, CMaterial *>					Materials;
			CMap<CString, CTexture *>					Textures;
			CMap<CString, CLight *>						Lights;

			CMaterial *									LoadStandardMaterial(CXon * root, CLight * l);
			CMaterial *									LoadBlendMaterial	(CXon * root, CLight * l);
			CTexture *									LoadTexture2d		(CXon * bitmapNode);
			CTexture *									LoadCubeTexture		(CXon * n);
			CTexture *									LoadVolumeTexture	(CXon * fileNode);
			CLight *									ImportLight			(const CString & file);
	};
}

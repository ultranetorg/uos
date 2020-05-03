#pragma once
#include "Geometry.h"
//#include "IViewStore.h"
#include "IMeshStore.h"
#include "Material.h"

namespace uos
{
	enum EVisualUpdate
	{
		EVisualUpdate_Obb = 1,
	};

	class CVisualGraphSpace;
	class CDirectPipeline;
	class CMaterialFactory;

	class UOS_ENGINE_LINKING CVisual : public CEngineEntity, public CShared
	{
		public:
			CString										Name;
			bool										Enabled = true;
			ETransformating								Transformating = ETransformating::Inherit;
			CMatrix										Matrix;
			CMatrix										FinalMatrix = CMatrix::Nan;
			ECullMode									CullMode = ECullMode::CCW;
			CAABB										Obb;
			CVisual *									Parent = null;
			CMesh *										Mesh = null;
			CMaterial *									Material = null;
			CMaterial *									InheritableMaterial =null;
			DWORD										Updates = 0;
			CRefList<CVisual *>							Nodes;
			CMesh *										ClippingMesh = null;
			bool										PixelPrecision = false;

			CDirectPipeline	*							Pipeline = null;
			float										CameraDistance;
			int											StencilIndex = -1;
			EClipping									Clipping = EClipping::Inherit;

			CEvent<>										FinalMatrixChanged;

			#ifdef _DEBUG
			CString										FullName;
			CTransformation								_Decomposed;
			CFloat3										_SSPosition;
			#endif

			bool										ShaderChanged = false;

			CDirectConstantBuffer *						Buffer = null;

			int											CameraPositionSlot = -1;
			int											WSlot = -1;				
			int											WVPSlot = -1; 				

			UOS_RTTI		
			CVisual(CEngineLevel * l, const CString & name, CMesh * mesh, CMaterial * mtl, const CMatrix & m);
			~CVisual();

			void										AddNode(CVisual *);
			void										InsertNode(CVisual *, CList<CVisual *>::iterator where);
			void										RemoveNode(CVisual *);

			void										SetMaterial(CMaterial * material);
			void										SetInheritableMaterial(CMaterial * material);
			CMaterial *									GetMaterial();
			bool										IsReady();

			void										Enable(bool e);
			bool										IsEnabled();
			void										SetName(const CString & name);
			CString &									GetName();

			void										SetCullMode(ECullMode c);
			ECullMode									GetCullMode();
			CAABB										GetFinalBBox(const CMatrix & m);
			CAABB										GetClippingBBox(const CMatrix & m);

			void										SetMatrix(const CMatrix & m);
			CMatrix	&									GetMatrix();
			void										SetMatrixPosition(CFloat3 & p);
			void										TransformMatrix(CMatrix & m);
			CAABB										GetAABB();
			CRect										GetProjectionRect(CCamera * vp);

			void										SetMesh(CMesh * mesh);
			CMesh *										GetMesh();

			void										SetClipping(CMesh * mesh);
			CMesh *										GetClipping();
			CVisual *									GetActualClipper();
			EClipping									GetActualClipping();

			CString										GetStatus();
			CString										GetClippingStatus();

			void										Save(CXon * r, IMeshStore * sm, IMaterialStore * st);
			void										Load(CXon * r, IMeshStore * sm, IMaterialStore * st, CMaterialFactory * f);
			CVisual *									Clone();

			void										Apply(CDirectDevice * d, CMatrix & sm, CCamera * c);

			template<class P> void Do(P p)
			{
				p(this);
				for(auto i : Nodes)
				{
					i->Do(p);
				}
			}
	};

}
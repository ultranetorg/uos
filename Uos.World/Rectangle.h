#pragma once
#include "Element.h"

namespace uos
{
	class UOS_WORLD_LINKING CRectangle : public CElement
	{
		public:
			using CElement::UpdateLayout;

			CFloat4										DiffuseColor;

			CSolidRectangleMesh *						InnerMesh = null;
			CVisual *									VBorder = null;
			CVisual *									VInner = null;
			//CActive *									AContent = null;

			bool										EnableActiveBody = true;

			CMaterial *									BorderMaterial = null;

			UOS_RTTI
			CRectangle(CWorldLevel * l, const CString & name = GetClassName());
			virtual ~CRectangle();

			void										UseInner();
			void										UseCanvas(CShader * s);

			virtual void								UpdateLayout(CLimits const & l, bool apply) override;

			void										UseClipping(EClipping c, bool content);

			void										ApplyBorder();
			void										ApplyBody();


			bool IsVisibleY(CElement * p, CElement * n)
			{
				//if(n->Transformation.Position.x > p->IW)
				//	return false;
				//
				//if(n->Transformation.Position.x + n->W < 0)
				//	return false;

				if(n->Transformation.Position.y > p->IH)
					return false;

				if(n->Transformation.Position.y + n->H < 0)
					return false;

				return true;
			}

			bool IsVisible(CElement * p, CElement * n)
			{
				if(n->Transformation.Position.x > p->IW)
					return false;
	
				if(n->Transformation.Position.x + n->W < 0)
					return false;

				if(n->Transformation.Position.y > p->IH)
					return false;

				if(n->Transformation.Position.y + n->H < 0)
					return false;

				return true;
			}



			void virtual LoadProperties(CStyle * s, CXon * n) override;
	};
}

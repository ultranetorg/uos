#pragma once
#include "WorldLevel.h"
#include "MaterialStore.h"
#include "MeshStore.h"
#include "Layout.h"

namespace uos
{
	class CUnit;

	class UOS_WORLD_LINKING CElement : public virtual IType, public virtual CShared
	{
		public:
			CString										Name;
			bool										Enabled;
			CTransformation								Transformation;
			CRefList<CElement *>						Nodes;
			CElement *									Parent;
			CVisual *									Visual;
			CActive *									Active;
			CSize 										Size = CSize::Nan;
			CWorldLevel *								Level;
			CExpressionList								Expressions;
			//CFloat3										ParentO = CFloat3::Nan;
			CTexture *									Texture = null;
			CSolidRectangleMesh *						CanvasMesh = null;
			
			float										W;
			float										H;
			float										D;
			float										IW;
			float										IH;
			CFloat6										M = CFloat6(0.f);
			CFloat6										B = CFloat6(0.f);
			CFloat6										P = CFloat6(0.f);

			CFloat3										O = CFloat3(0.f);
			CSize										C;
			
			CLimits										Slimits = {CSize::Nan, CSize::Nan};
			CLimits										Climits = {CSize::Nan, CSize::Nan};

			UOS_RTTI
			CElement(CWorldLevel * l, const CString & name = GetClassName());

			void *										FindField(CString const & name);
			virtual void 								Express(CString const & f, std::function<CSize(bool)> e);
			virtual void								Express(CString const & f, std::function<float()> e);
			virtual void								Express(CString const & f, std::function<CFloat6()> e);
			void										Reset(CString const & f);

			virtual void								Enable(bool e);
			void										SetName(const CString & name);

			void										TransformX(float x);
			void										TransformY(float y);
			void										TransformZ(float z);
			void										Transform(float x, float y);
			void										Transform(float x, float y, float z);
			void										Transform(CFloat3 & p);
			virtual void								Transform(const CTransformation & t);
			CTransformation								GetFinalTransformation();
			CFloat3										GetVisualPoint(CElement * in, CFloat3 & p);
			void										UpdateMatrixes();

			void virtual 								AddNode(CElement * n);
			void virtual 								RemoveNode(CElement * n);
			void										ClearNodes();
			CElement *									GetNode(const CString & path);
			CElement *									GetRoot();
			CElement *									FindCommonAncestor(CElement * n);
			bool										IsDescendant(CElement * n);
			bool										IsAncestor(CElement * n);

			virtual CElement *							Clone();

			void		  								UpdateLayout();
			void virtual 								UpdateLayout(CLimits const & l, bool apply);
			void virtual								ProcessLayoutChanges(CElement * s);
			void virtual								PropagateLayoutChanges(CElement * s);
			static CSize								CalculateSize(CRefList<CElement *> & nodes);

			void										SaveBasic(CXon * r, IMeshStore * mhs, IMaterialStore * mts);
			void										LoadBasic(CXon * p, IMeshStore * mhs, IMaterialStore * mts);

			void										Load(CStyle * s, CString & u);
			virtual void								LoadNested(CStyle * s, CXon * n, std::function<CElement *(CXon *, CElement *)> & load);
			virtual void								LoadProperties(CStyle * s, CXon * n);
			void										ApplyStyles(CStyle * s, CList<CString> const & classes);

			CUnit *										GetUnit();

			virtual void								SetEntity(IEntity * e){}
			bool										InPadding(CFloat3 & p);
			bool										InBorder(CFloat3 & p);
			bool										InMargin(CFloat3 & p);

			virtual CString								GetStatus();

			float										WtoIW(float w);
			float										IWtoW(float iw);
			float										HtoIH(float h);
			float										IHtoH(float ih);

			virtual void								UseCanvas(CVisual * v, CMesh * mesh, CShader * s);
			void										UpdateCanvas();
			virtual	void								Draw();

			template<class T> T * One(CString const & name)
			{
				for(auto i : Nodes)
				{
					if(i->Name == name)
					{
						return dynamic_cast<T *>(i);
					}

					if(auto n = i->One<T>(name))
					{
						return n;
					}
				}

				return null;
			}

			CElement * One(CString const & name)
			{
				for(auto i : Nodes)
				{
					if(i->Name == name)
					{
						return i;
					}

					if(auto n = i->One(name))
					{
						return n;
					}
				}

				return null;
			}


			template<class P> void Do(P p)
			{
				p(this);
				for(auto i : Nodes)
				{
					i->Do(p);
				}
			}
			
			template<class T> T * GetAncestorOf()
			{
				auto p = this;
				while(p && dynamic_cast<T *>(p) == null)
				{
					p = p->Parent;
				}
				return dynamic_cast<T *>(p);
			}


		protected:
			virtual ~CElement();

	};
}

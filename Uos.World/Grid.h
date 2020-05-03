#pragma once
#include "Text.h"
#include "Rectangle.h"
#include "Scrollbar.h"

namespace uos
{
	class CGridColumn;
	class CGridRow;

	class CGridCell
	{
		public:
			CElement *			Viewer = null;
			CElement *			Editor = null;
			CElement *			Current = null;
			CGridColumn *		Column = null;
			CGridRow *			Row = null;
			bool				NeedUpdate = true;
			IComparable *		Entity = null;

			~CGridCell()
			{
				Viewer->Free();
			}
	};

	class CGridColumn
	{
		public:
			CString							Name;
			int								Index = -1;
			CElement *						Viewer = null;
			CElement *						Editor = null;
			float							W = 100.f;
			EOrder							Order = EOrder::Null;

			CGridColumn(CWorldLevel * l, CStyle * s)
			{
				Viewer = new CText(l, s);
			}

			~CGridColumn()
			{
				Viewer->Free();
			}
	};

	class CGridRow
	{
		public:
			CArray<CGridCell *>				Cells;
			CArray<CGridColumn *> &			Columns;
			float							H = 0.f;

			CGridRow(CArray<CGridColumn *> & cols) : Columns(cols)
			{
			}

			~CGridRow()
			{
				for(auto i : Cells)
				{
					delete i;
				}
			}

			CGridCell * AddCell(IEntity * e)
			{
				auto c = new CGridCell();
				c->Entity = dynamic_cast<IComparable *>(e);
				c->Row = this;
				c->Column = Columns[Cells.size()];
				c->Viewer = c->Column->Viewer->Clone();
				c->Current = c->Viewer;
				c->Current->SetEntity(e);

				c->Current->Visual->Clipping = EClipping::Apply;
				c->Current->Active->Clipping = EClipping::Apply;

				CSize s(c->Column->W, FLT_MAX, 0);
				c->Current->UpdateLayout({s, s}, true);
				Cells.push_back(c);
				return c;
			}
	};

	class CGridHeader : public CText
	{
		public:
			CArray<CGridColumn *>						Columns;
			CFloat2										Spacing;

			UOS_RTTI
			CGridHeader(CWorldLevel * l, CStyle * s, CString const & name = GetClassName()) : CText(l, s, name)
			{
				Visual->Clipping = EClipping::Apply;
				Active->Clipping = EClipping::Apply;

				auto mtl = new CMaterial(&Engine->EngineLevel, Level->Engine->PipelineFactory->DiffuseTextureShader);
				mtl->Textures[L"DiffuseTexture"] = Texture;	
				mtl->Samplers[L"DiffuseSampler"].SetFilter(ETextureFilter::Point, ETextureFilter::Point, ETextureFilter::Point);

				Visual->SetMaterial(mtl);

				mtl->Free();

				Express(L"C",	[this](auto apply)
								{
									auto w = Columns.Sum<float>([](auto i){ return i->W; });
									auto ws = max(0, Spacing.x * (Columns.size() - 1));
				
									return CSize(w + ws, Font->Height, 0);
								});
			}

			~CGridHeader()
			{
				for(auto i : Columns)
				{
					delete i;
				}
			}
			
			virtual void Draw() override
			{
			 	auto c = Texture->BeginDraw();
				c->Clear(CFloat4(0));

				auto b = c->CreateSolidBrush(Style->Get<CFloat4>(L"Border/Material"));

				auto x = 0.f;

				for(auto i : Columns)
				{
					c->FillRectangle(CRect(x, 0.f, i->W, float(Texture->H)), b);
					c->DrawText(i->Name, Font, CFloat4(0, 0, 0, 1), CRect(x, 0, i->W, float(Texture->H)), EXAlign::Center, EYAlign::Center, false, true);
					x += i->W + Spacing.x;
				}

				b->Free();
				Texture->EndDraw();
			}

	};

	class CGridBody : public CRectangle
	{
		public:
			CArray<CGridColumn *> &						Columns;
			CFloat2										Spacing;

			UOS_RTTI
			CGridBody(CWorldLevel * l, CStyle * s, CArray<CGridColumn *> & cols, CString const & name = GetClassName()) : CRectangle(l, name), Columns(cols)
			{
				UseClipping(EClipping::Inherit, true);
			}

			~CGridBody()
			{
			}
	};

	class UOS_WORLD_LINKING CGrid : public CRectangle
	{
		public:
			CFloat2										Spacing = CFloat2(0.f);
			
			CArray<CGridRow *>							Rows;
			CStyle *									Style;
			CGridHeader *								Header;
			CScrollbar *								Scrollbar;
			CGridBody *									Body;

			UOS_RTTI
			CGrid(CWorldLevel * l, CStyle * s, CString const & name = GetClassName());
			~CGrid();
			
			using CElement::UpdateLayout;

			CGridRow *									AddRow();
			CGridColumn *								AddColumn(CString const & name, float w, EOrder o = EOrder::Null);

			void										SetSpacing(CFloat2 & s);

			void										OnScrolled();

			void										Arrange(bool sort);
			virtual void								UpdateLayout(CLimits const & l, bool apply) override;
	};
}

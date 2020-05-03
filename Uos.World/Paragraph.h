#pragma once
#include "Text.h"

namespace uos
{
	class CLineBreak : public CElement
	{
		public:
			UOS_RTTI
			CLineBreak(CWorldLevel * l) : CElement(l, GetClassName())
			{
				auto h = Level->Style->GetFont(L"Text/Font")->Height;
				
				Express(L"W", []{ return 0.f; });
				Express(L"H", [h]{ return h; });

				UpdateLayout(CLimits::Empty, false);
			}
	};

	class CParagraph : public CRectangle
	{
		public:
			float					FontH;
					
			UOS_RTTI
			CParagraph(CWorldLevel * l, CString const & name = GetClassName()) : CRectangle(l, name)
			{

				Express(L"C",	[this](auto apply)
								{
									auto l = Climits;

									float x = 0, y = 0, hmax = 0;
									float h = 0;
									float w = 0;

									for(auto i : Nodes)
									{
										if(auto b = i->As<CLineBreak>())
										{
											h += hmax;
											y -= hmax;
											w = max(x, w);
											x = 0;
											hmax = b->H;
										}
										else
										{
											i->UpdateLayout(l, apply);
											i->Transform(x, y - i->Size.H, Z_STEP);

											x += i->Size.W;
											hmax = max(hmax, i->Size.H);
										}

										if(i == Nodes.back())
										{
											h += hmax;
											w = max(x, w);
										} 
									}
									
									for(auto i : Nodes)
									{
										i->TransformY(i->Transformation.Position.y + h);
									}

									return CSize(w, h, 0);
								});
			}
			~CParagraph()
			{
				///for(auto i : Nodes)
				///{
				///	if(i->Get<CLineBreak>() == null)
				///		RemoveNode(i);
				///}
			}

			CText *	Add(CString const & text, CFloat4 & color)
			{
				auto t = new CText(Level, Level->Style);
				t->SetText(text);
				t->SetColor(color);
				AddNode(t);
				t->Free();
				return t;
			}

			CText *	Add(CString const & text)
			{
				auto t = new CText(Level, Level->Style);
				t->SetText(text);
				AddNode(t);
				t->Free();
				return t;
			}

			CText *	Insert(CElement * after, CString const & text, bool active = false)
			{
				auto t = new CText(Level, Level->Style, L"", active);
				t->SetText(text);
				AddNode(t);
				t->Free();
				return t;
			}

			void Insert(CElement * e, CElement * after)
			{
				AddNode(e);
			}
			
			void InsertBreak(CElement * after)
			{
				auto e = new CLineBreak(Level);
				AddNode(e);
				e->Free();
			}

			CLineBreak * AddBreak()
			{
				auto t = new CLineBreak(Level);
				AddNode(t);
				t->Free();
				return t;
			}
	};
}

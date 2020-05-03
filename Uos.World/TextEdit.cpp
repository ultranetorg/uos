#include "StdAfx.h"
#include "TextEdit.h"

using namespace uos;

CTextEdit::CTextEdit(CWorldLevel * l, CStyle * s, const CString & name) : CRectangle(l, name)
{
	Style = s;

	UseCanvas(l->Engine->PipelineFactory->TextShader);
	UseClipping(EClipping::Inherit, true);

	auto m = new CSolidRectangleMesh(&Level->Engine->EngineLevel);
	auto mtl = new CMaterial(&Level->Engine->EngineLevel, Level->Engine->PipelineFactory->DiffuseColorShader);
	mtl->Float4s[L"DiffuseColor"] = s->Get<CFloat4>(L"TextEdit/Caret/ColorA");
	Caret = new CVisual(&Level->Engine->EngineLevel, L"Caret", m, mtl, CMatrix());
	//Caret->Clipping = EClipping::Apply;
	m->Free();
	mtl->Free();

	Font	= s->GetFont(L"Text/Font");
/*
	//Mesh	= new CSolidRectangleMesh(&l->Engine->EngineLevel);
	Texture = Level->Engine->TextureFactory->CreateTexture();
		
	mtl = new CMaterial(&Level->Engine->EngineLevel, Level->Engine->PipelineFactory->TextShader);
	mtl->Textures[L"DiffuseTexture"] = Texture;	
	mtl->Samplers[L"DiffuseSampler"].SetFilter(ETextureFilter::Point, ETextureFilter::Point, ETextureFilter::Point);
	mtl->Float4s[L"Color"] = s->Get<CFloat4>(L"TextEdit/Text/Color");
	
	VContent->SetMaterial(mtl);
	VContent->SetMesh(InnerMesh);
	//VContent->PixelPrecision = true;
	mtl->Free();
*/
	VInner->Material->Float4s[L"Color"] = s->Get<CFloat4>(L"TextEdit/Text/Color");


	///Visual->SetMaterial(s->GetMaterial(L"TextEdit/Background/Material"));
	///BorderMaterial = s->GetMaterial(L"TextEdit/Border/Material");
	///Express(L"B", [s]{ return CFloat6(s->Get<CFloat>(L"TextEdit/Border/Thickness")); });

	Active->MouseInput += ThisHandler(OnMouse);
	Active->KeyboardInput += ThisHandler(OnKeyboard);
	Active->StateChanged += ThisHandler(OnStateChanged); 

	Lines.push_back(L"");

	SetMultiline(false);
	SetFont(s->GetFont(L"Text/Font"));
}

CTextEdit::~CTextEdit()
{
	Caret->Free();

	for(auto i : Selections)
	{
		Visual->RemoveNode(i);
	}
	//Texture->Free();
	//Mesh->Free();
}

void CTextEdit::SetColor(CFloat3 & c)
{
	Color = c;
}

void CTextEdit::SetMultiline(bool e)
{
	IsMultiLine = e;

	if(!e)
	{
		Express(L"IH", [this]{ return Font->Height; });
	}
}

void CTextEdit::OnStateChanged(CActive * s, CActive *, CActiveStateArgs * a)
{
	if(a->New == EActiveState::Active)
	{
		Visual->AddNode(Caret);
		Level->Core->AddWorker(this);
	}
	if(a->Old == EActiveState::Active)
	{
		Visual->RemoveNode(Caret);
		Level->Core->RemoveWorker(this);
	}
}

void CTextEdit::OnMouse(CActive *, CActive *, CMouseArgs * a)
{
	if(a->Control == EControl::Wheel && a->Action == EInputAction::Rotation)
	{
		int n = (int)floor(IH/Font->Height);
		int range = (int)Lines.size() - n;
	
		if(range > 0)
		{
			YOffset += (int)a->Input->Position.y * -3;
			
			YOffset = max(0, YOffset);
			YOffset = min(range, YOffset);
		
			Draw();
		}
	}

	if(a->Control == EControl::LeftButton && a->Action == EInputAction::On)
	{
		auto p = a->Pick.Point;
		p = (a->Pick.Active->FinalMatrix * Visual->FinalMatrix.GetInversed()).TransformCoord(p);
	
		auto y = GetLine(p.y);
		if(y >= 0)
		{
			CaretPos.y = y;
			CaretPos.x = GetPosition(Lines[CaretPos.y], Xscroll + p.x);
			UpdateCaret();
		
			SelA = SelB;
			UpdateSelection();
		}
	}
}

void CTextEdit::OnKeyboard(CActive *, CActive *, CKeyboardArgs * a)
{

	bool update = false;


	if(a->Class == EInputClass::Keyboard && a->Action == EInputAction::On)
	{
		if(IsSelecting && !(GetAsyncKeyState(VK_SHIFT) & 0b1000'0000'0000'0000))
		{	
			IsSelecting = false;
			SelA = SelB;
			UpdateSelection();
		}
		
		if(GetAsyncKeyState(VK_SHIFT) & 0b1000'0000'0000'0000 && !IsSelecting)
		{
			switch(a->Control)
			{
				case VK_RIGHT:
				case VK_LEFT:
				case VK_UP:
				case VK_DOWN:
				case VK_END:
				case VK_HOME:
					SelA = CaretPos;
					IsSelecting = true;
					break;
			}
		}
		
		switch(a->Control)
		{
			case VK_RIGHT:
				if(CaretPos.x < (int)Lines[CaretPos.y].size())
				{
					CaretPos.x++;
					UpdateCaret();
				}
				else if(IsMultiLine && CaretPos.x == Lines[CaretPos.y].Count() && CaretPos.y < Lines.Count()-1)
				{
					CaretPos.y++;
					CaretPos.x = 0;
					UpdateCaret();
				}
				break;
			
			case VK_LEFT:
				if(CaretPos.x > 0)
				{
					CaretPos.x--;

					UpdateCaret();
				}
				else if(IsMultiLine && CaretPos.x == 0 && CaretPos.y > 0)
				{
					CaretPos.y--;
					CaretPos.x = (LONG)Lines[CaretPos.y].size();
					UpdateCaret();
				}
				break;

			case VK_DOWN:
				if(IsMultiLine && CaretPos.y < Lines.Count() - 1)
				{
					CaretPos.y += 1;
					CaretPos.x = GetPosition(Lines[CaretPos.y], Caret->Matrix._41 + Xscroll);
					UpdateCaret();
				}
				break;

			case VK_UP:
				if(IsMultiLine && CaretPos.y > 0)
				{
					CaretPos.y -= 1;
					CaretPos.x = GetPosition(Lines[CaretPos.y], Caret->Matrix._41 + Xscroll);
					UpdateCaret();
				}
				break;

			case VK_NEXT:
				if(IsMultiLine && CaretPos.y < Lines.Count() - 1)
				{
					CaretPos.y += int(IW/Font->Height);

					if(CaretPos.y >= Lines.Count())
					{
						CaretPos.y = Lines.Count() - 1;
					}

					CaretPos.x = GetPosition(Lines[CaretPos.y], Caret->Matrix._41 + Xscroll);
					UpdateCaret();
				}
				break;

			case VK_PRIOR:
				if(IsMultiLine && CaretPos.y > 0)
				{
					CaretPos.y -= int(IW/Font->Height);
					if(CaretPos.y < 0)
					{
						CaretPos.y = 0;
					}
					CaretPos.x = GetPosition(Lines[CaretPos.y], Caret->Matrix._41 + Xscroll);
					UpdateCaret();
				}
				break;
			
			case VK_END:
				CaretPos.x = (LONG)Lines[CaretPos.y].size();
				UpdateCaret();
				break;

			
			case VK_HOME:
				CaretPos.x = 0;
				UpdateCaret();
				break;

			case VK_DELETE:
				if(CaretPos.x < Lines[CaretPos.y].Count())
				{
					Lines[CaretPos.y].erase(CaretPos.x, 1);
					update = true;
				}
				else if(IsMultiLine && CaretPos.x == Lines[CaretPos.y].Count() && CaretPos.y < Lines.Count()-1)
				{
					Lines[CaretPos.y] += Lines[CaretPos.y+1];
					Lines.erase(Lines.begin() + CaretPos.y+1);
					update = true;

				}
				break;

			case VK_INSERT:
				if(GetAsyncKeyState(VK_SHIFT) & 0b1000'0000'0000'0000)
				{
					if(!IsClipboardFormatAvailable(CF_UNICODETEXT)) 
						return; 

					if(!OpenClipboard(NULL)) 
						return; 
 
					auto hglb = GetClipboardData(CF_UNICODETEXT); 
					if(hglb != NULL) 
					{ 
						wchar_t * lptstr = (wchar_t *)GlobalLock(hglb); 
						if(lptstr != NULL) 
						{ 
							Insert(lptstr);
							GlobalUnlock(hglb); 
						} 
					} 
					CloseClipboard(); 
				}
				break;

			case VK_BACK:
				if(CaretPos.x > 0)
				{
					CaretPos.x--;

					Lines[CaretPos.y].erase(CaretPos.x, 1);
					update = true;
				}
				else if(IsMultiLine && CaretPos.x == 0 && CaretPos.y > 0)
				{
					CaretPos.x = (LONG)Lines[CaretPos.y - 1].size();

					Lines[CaretPos.y - 1] += Lines[CaretPos.y];
					Lines.erase(Lines.begin() + CaretPos.y);

					CaretPos.y--;

					update = true;
				}
				UpdateCaret();
				break;

			case VK_RETURN:
			{
				if(IsMultiLine)
				{
					CString s;
					if(CaretPos.x < (int)Lines[CaretPos.y].size())
					{
						s = Lines[CaretPos.y].substr(CaretPos.x);
						Lines[CaretPos.y].erase(CaretPos.x);
					}

					Lines.insert(Lines.begin() + CaretPos.y + 1, s);

					CaretPos.y += 1;
					CaretPos.x = 0;

					update = true;
				}
				break;
			}
			default:
			{
				Lines[CaretPos.y].insert(CaretPos.x, 1, (wchar_t)a->Control);
				CaretPos.x++;

				UpdateCaret();

				update = true;
			}
		}
	}

	if(AdjustCaret())
	{
		update = true;
	}

	if(GetAsyncKeyState(VK_SHIFT) & 0b1000'0000'0000'0000 && IsSelecting)
	{
		switch(a->Control)
		{
			case VK_RIGHT:
			case VK_LEFT:
			case VK_UP:
			case VK_DOWN:
			case VK_END:
			case VK_HOME:
				SelB = CaretPos;
				UpdateSelection();
				break;
		}
	}

	if(update)
	{
		Draw();
	}
	
}

bool CTextEdit::AdjustCaret()
{
	bool update = false;

	int n = (int)floor(IH/Font->Height);

	if(CaretPos.y - YOffset >= n)
	{
		YOffset = CaretPos.y - n + 1;
		update = true;
	}

	if(CaretPos.y - YOffset < 0)
	{
		YOffset = CaretPos.y;
		update = true;
	}

	if(Caret->Matrix._41 > IW)
	{
		Xscroll = Caret->Matrix._41 + Xscroll - IW + Font->TextMetric.tmMaxCharWidth * 3;
		update = true;
	}

	if(Caret->Matrix._41 < 0)
	{
		Xscroll += Caret->Matrix._41 - Font->TextMetric.tmMaxCharWidth * 3;
		Xscroll = max(Xscroll, 0);
		update = true;
	}

	return update;
}

void CTextEdit::Insert(const CString & t)
{
	auto lines = t.ToLines();

	CString e = Lines[CaretPos.y].substr(CaretPos.x);

	auto i = lines.begin();

	Lines[CaretPos.y].erase(CaretPos.x);
	Lines[CaretPos.y] += *i;

	CaretPos.x += (LONG)i->size();

	i++;

	for(;i != lines.end(); i++)
	{
		Lines.insert(Lines.begin() + CaretPos.y + 1, *i);
		CaretPos.y++;
		CaretPos.x = (LONG)i->size();
	}

	Lines[CaretPos.y] += e;
	
	Draw();

}

void CTextEdit::Select(int ay, int ax, int by, int bx)
{
	SelA.x = ax;
	SelA.y = ay;

	SelB.x = bx;
	SelB.y = by;

	UpdateSelection();
}

void CTextEdit::UpdateSelection()
{
	if(SelA.x != SelB.x || SelA.y != SelB.y)
	{
		auto sa = SelA;
		auto sb = SelB;

		if(sa.y > sb.y)
		{
			std::swap(sa, sb);
		}
		if(sa.y == sb.y && sa.x > sb.x)
		{
			std::swap(sa, sb);
		}

		if(sa.y < YOffset)
		{
			sa.y = YOffset;
			sa.x = 0;
		}

		int N = unsigned int(ceil(IW/Font->Height));

		if(sb.y > YOffset + N)
		{
			sb.y = YOffset + N;
			sb.x = Lines[YOffset + N].Count();
		}

		for(int i = Selections.Size(); i < sb.y - sa.y + 1; i++)
		{
			auto m = new CSolidRectangleMesh(&Level->Engine->EngineLevel);
					
			auto v = new CVisual(&Level->Engine->EngineLevel, L"selection", m, Level->Materials->GetMaterial(L"0 0 1"), CMatrix());
			v->Clipping = EClipping::Apply;

			Selections.Add(v);

			Visual->AddNode(v);

			Visual->Nodes.Remove(v);
			Visual->Nodes.AddFront(v);

			v->Free();
			m->Free();
		}

		auto i = Selections.begin();
		for(int y=sa.y; y <= sb.y; y++)
		{
			auto a = y == sa.y ? sa.x : 0;
			auto b = y == sb.y ? sb.x : Lines[y].Count();

			float x = 0.f;
			for(int i = 0; i<a; i++)
			{
				x += Font->Widths[Lines[y][i]];
			}	
	
			float w = 0.f;
			for(auto i = a; i < b; i++)
			{
				w += Font->Widths[Lines[y][i]];
			}
			
			(*i)->Mesh->As<CSolidRectangleMesh>()->Generate(0, 0, w, Font->Height);
			(*i)->SetMatrix(CMatrix::FromPosition(x - Xscroll, IH - (y - YOffset + 1) * Font->Height, Z_STEP));
			i++;
		}

		while(i != Selections.end())
		{
			(*i)->Mesh->As<CSolidRectangleMesh>()->Generate(0, 0, 0, 0);
			i++;
		}
	}
	else
	{
		for(auto i : Selections)
		{
			Visual->RemoveNode(i);
		}
		Selections.Clear();
	}
}

void CTextEdit::UpdateCaret()
{
	int x = 0;
	auto t = Font->Widths[L'\t'] * TabCount; 

	for(int i=0; i<CaretPos.x; i++)
	{
		if(Lines[CaretPos.y][i] != L'\t')
			x += Font->Widths[Lines[CaretPos.y][i]];
		else
		{
			if(x % int(t) == 0)
				x += t;
			else
				x += t - x % int(t);
		}
	}

	auto xx = x - Xscroll;
	auto yy = IsMultiLine ? IH - Font->Height * (CaretPos.y - YOffset + 1) : (IH - Font->Height)/2;

	Caret->SetMatrix(CMatrix::FromPosition(O.x + xx, O.y + yy, Z_STEP * 3));
	Caret->Enable(0 <= xx && xx <= IW - 1 && 0 <= yy && yy <= IH - Font->Height);
}

void CTextEdit::Draw()
{
	if(Font)
	{
		float w = float(Texture->W);
		float h = float(Texture->H);
	
		auto c = Texture->BeginDraw();
		
		c->TabLength = TabCount;
		c->Clear(CFloat4(0));

		auto line = Lines.begin();
		std::advance(line, YOffset);
		auto y = IsMultiLine ? h : h - (h - Font->PixelHeight)/2;

		while(line != Lines.end() && y + Font->PixelHeight > 0)
		{
			c->DrawText(*line, Font, CFloat4(1), CRect(-Xscroll, y - Font->PixelHeight, FLT_MAX, Font->PixelHeight), EXAlign::Left, EYAlign::Center, false, false);

			line++; 
			y -= Font->PixelHeight;
		}

		Texture->EndDraw();

		UpdateCaret();
		UpdateSelection();
	}
}

void CTextEdit::SetText(const CString & t)
{
	Lines = IsMultiLine ? t.ToLines() : CArray<CString>{t};

	CaretPos.x = 0;
	CaretPos.y = 0;
}

CString CTextEdit::GetText()
{
	return IsMultiLine ? CString::Join(Lines, [](auto & i){ return i; }, L"\r\n") : Lines[0];
}


void CTextEdit::SetFont(CFont * f)
{
	if(Font != f)
	{
		Font = f;
	}
	Caret->Mesh->As<CSolidRectangleMesh>()->Generate(0, 0, 1, Font->Height);
}

void CTextEdit::DoIdle()
{
	auto t = Level->Core->Timer.GetTime();
	if(t - CaretTime > 0.3)
	{
		CaretTime = t;
		Caret->Material->Float4s[L"DiffuseColor"] =  CaretBlinkPhase ? CFloat4(1, 1, 1, 1) : CFloat4(1, 0, 0, 1);
		CaretBlinkPhase = !CaretBlinkPhase;
	}
}

int CTextEdit::GetLine(float y)
{
	return IsMultiLine ? min(YOffset + int(floor((IH - y)/Font->Height)), Lines.Count()-1) : 0;
}

int CTextEdit::GetPosition(CString & s, float x)
{
 	int w = 0;
 	unsigned int i = 0;
 	long c;
 
 	auto t = int(TabCount * Font->Widths[L'\t']);
 
 	for(; i < s.size(); i++)
 	{	
 		if(s[i] == L'\t' && t > 0)
 			if(w % t == 0)
 				c = t;
 			else
 				c = t - w % t;
 		else
 			c = Font->Widths[s[i]];
 
 		if(w <= x && x <= w + c )
 		{
 			if(x - w < c * 0.5f)
 			{
 				break;
 			}
 			else
 			{
 				i++;
 				break;
 			}
 		}
 		w += c;
 	}
 
 	return i;
}

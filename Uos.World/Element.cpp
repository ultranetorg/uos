#include "StdAfx.h"
#include "Element.h"
#include "IUwmProtocol.h"
#include "Unit.h"

using namespace uos;

CElement::CElement(CWorldLevel * l, const CString & name)
{
	Level			= l;
	Name			= name;
	Enabled			= true;
	Parent			= null;

	auto i = Name.find(L'-');
	if(i != CString::npos)
	{
		Name.resize(i);
	}

	Visual	= new CVisual(&l->Engine->EngineLevel, Name, null, null, CMatrix());
	Active	= new CActive(&l->Engine->EngineLevel, Name);
	   
	Active->Owner = this;

	Express(L"IW", [this]{ return C.W; });
	Express(L"IH", [this]{ return C.H; });

	Transform(CTransformation(0, 0, Z_STEP));
}


CElement::~CElement()
{
	if(Texture)
	{
		Texture->Free();
	}

	Active->Owner = null;

	if(Parent != null)
	{
		DebugBreak();
	}

	ClearNodes();

	Visual->Free();
	Active->Free();
}
	
void CElement::UpdateLayout()
{
	UpdateLayout(Slimits, true);
}

void CElement::UpdateLayout(CLimits const & l, bool apply)
{
	Slimits = l;
	W = H = IW = IH = C.W = C.H = NAN;
	auto o = O;
	O = {NAN, NAN, 0};

	float rt, tp;

	auto eval = [this, &rt, &tp](bool apply)
				{
					///Expressions.Execute();
					for(auto & i : Expressions.Floats)
						*i.first = i.second();

					for(auto & i : Expressions.Float6s)
						*i.first = i.second();

					for(auto & i : Expressions.Sizes)
						if(i.first != &C)
							*i.first = i.second(apply);

					O.x = M.LF + B.LF + P.LF;
					O.y = M.BM + B.BM + P.BM;
					rt = M.RT + B.RT + P.RT;
					tp = M.TP + B.TP + P.TP;

					if(Expressions.Contains(&W))	IW = W - (O.x + rt); else 
					if(Expressions.Contains(&IW))	W = IW + (O.x + rt);
					
					if(Expressions.Contains(&H))	IH = H - (O.y + tp); else 
					if(Expressions.Contains(&IH))	H = IH + (O.y + tp);

					#ifdef _DEBUG
					//if(W < 0 || H < 0 || IW < 0 || IH < 0)
					//{
					//	throw CException(HERE, L"W < 0 || H < 0 || IW < 0 || IH < 0");
					//}
					#endif

				};

	if(!Expressions.IsEmpty())
	{
		eval(false);
			
		CSize s;
		s.W = max(0, (isfinite(W) ? W : l.Smax.W) - (O.x + rt));
		s.H = max(0, (isfinite(H) ? H : l.Smax.H) - (O.y + tp));

		Climits = {s, s};

		if(Expressions.Sizes.Contains(&C))
			C = Expressions.Sizes[&C](apply);
		else
		{
			for(auto i : Nodes)
			{
				i->UpdateLayout(Climits, apply);
			}

			C = CSize::Empty;
		}
			
		W = H = IW = IH = NAN;
	
		eval(apply);
	
		Size = CSize(W, H, 0);

		if(o != O)
		{
			for(auto i : Nodes)
				i->UpdateMatrixes();
		}
	}
	else
		Size = CSize::Empty;
}

void CElement::PropagateLayoutChanges(CElement * s)
{
	if(Parent)
	{
		Parent->PropagateLayoutChanges(s);
	}
	else
	{
		ProcessLayoutChanges(s);
	}
}

void CElement::ProcessLayoutChanges(CElement * s)
{
	UpdateLayout();
}

void CElement::TransformX(float x)
{
	auto t = Transformation;
	t.Position.x = x;
	Transform(t);
}

void CElement::TransformY(float y)
{
	auto t = Transformation;
	t.Position.y = y;
	Transform(t);
}

void CElement::TransformZ(float z)
{
	auto t = Transformation;
	t.Position.z = z;
	Transform(t);
}

void CElement::Transform(float x, float y)
{
	auto t = Transformation;
	t.Position.x = x;
	t.Position.y = y;
	Transform(t);
}

void CElement::Transform(float x, float y, float z)
{
	auto t = Transformation;
	t.Position.x = x;
	t.Position.y = y;
	t.Position.z = z;
	Transform(t);
}

void CElement::Transform(CFloat3 & p)
{
	Transform(CTransformation(p));
}

void CElement::Transform(const CTransformation & t)
{
	Transformation = t;

	if(Transformation != t)
	{
		UpdateMatrixes();
	}
}

void CElement::UpdateMatrixes()
{
	auto m = CMatrix(Transformation);

	if(Parent)
	{
		m = m * CMatrix::FromPosition(Parent->O);
	}

	Visual->SetMatrix(m);
	Active->SetMatrix(m);
}

CTransformation CElement::GetFinalTransformation()
{
	auto t = Transformation;
	auto p = Parent;
	
	while(p != null)
	{
		t = t * p->Transformation;
		p = p->Parent;
	}
	return t;
}
	
CFloat3 CElement::GetVisualPoint(CElement * n, CFloat3 & p)
{
	return Visual->FinalMatrix.GetInversed().TransformCoord(n->Visual->FinalMatrix.TransformCoord(p));
}

void CElement::AddNode(CElement * n)
{
	if(n->Parent != null)
	{
		throw CException(HERE, L"Node is already a child");
	}
	if(FindCommonAncestor(n) != null)
	{
		throw CException(HERE, L"Node is already a part of hierarchy");
	}

	n->Parent = this;
	Nodes.Add(n);

	Visual->AddNode(n->Visual);
	Active->AddNode(n->Active);

	n->UpdateMatrixes();
}
 	
void CElement::RemoveNode(CElement * n)
{
	if(n->Visual->Parent == Visual)
		Visual->RemoveNode(n->Visual);
			
	if(n->Active->Parent == Active)
		Active->RemoveNode(n->Active);

	n->Parent = null;
	Nodes.Remove(n);
}

void CElement::ClearNodes()
{
	while(auto i = Nodes.First())
	{
		RemoveNode(i);
	}
}

CElement * CElement::GetNode(const CString & path)
{
	auto nodes = path.SplitToList(L"/");

	auto p = Nodes.Find([&nodes](CElement * i){ return i->Name == nodes.front();});
	if(p != null)
	{
		nodes.pop_front();

		for(auto i : nodes)
		{
			p = p->GetNode(i);
			if(p == null)
			{
				return null;
			}
		}
	}
	return p;	 
}	

CElement * CElement::GetRoot()
{
	CElement * r = this;
	while(r->Parent != null)
	{
		r = r->Parent;
	}
	return r;
}
	
CElement * CElement::FindCommonAncestor(CElement * n)
{
	auto a = this;
	while(a != null)
	{
		auto b = n;
		while(b != null)
		{
			if(a == b)
			{
				return a;
			}
			b = b->Parent;
		}
		a = a->Parent;
	}
	return null;
}

bool CElement::IsAncestor(CElement * n)
{
	CElement * p = this;
	while(p != null && p != n)
	{
		p = p->Parent;
	}
	return p != null;
}
	
bool CElement::IsDescendant(CElement * n)
{
	return n->IsAncestor(this);
}
	
void CElement::SetName(const CString & name)
{
	Name = name;

	auto i = Name.find(L'-');
	if(i != CString::npos)
	{
		Name.resize(i);
	}

	if(Visual)
	{
		Visual->SetName(Name);
	}
	if(Active)
	{
		Active->SetName(Name);
	}
}

void CElement::Enable(bool e)
{
	if(Enabled != e)
	{
		Visual->Enable(e);
		Active->Enable(e);

		for(auto n : Nodes)
		{
			n->Enable(e);
		}
//			MarkAsModifiedRecursively();
		Enabled = e;
	}
}

void CElement::SaveBasic(CXon * r, IMeshStore * mhs, IMaterialStore * mts)
{
	r->Set(Name);
	r->Add(L"T")->Set(Transformation);
	r->Add(L"Size")->Set(Size);
	Visual->Save(r->Add(L"Visual"), mhs, mts);
}

void CElement::LoadBasic(CXon * r, IMeshStore * mhs, IMaterialStore * mts)
{
	SetName(r->Get<CString>());
	Transformation = r->Get<CTransformation>(L"T");
	Size = r->Get<CSize>(L"Size");
	Visual->Load(r->One(L"Visual"), mhs, mts, Level->Engine->MaterialFactory);
}

CElement * CElement::Clone()
{
	auto n = new CElement(Level, Name);
	n->Enabled			= Enabled;
	n->Transformation	= Transformation;
	n->Size				= Size;
	n->Slimits			= Slimits;

	sh_free(n->Visual);
	n->Visual = Visual->Clone();

	return n;
}

CSize CElement::CalculateSize(CRefList<CElement *> & nodes)
{
	auto bb = CAABB::InversedMax;
	for(auto i : nodes)
	{
		bb.Join2D(i->Transformation, i->Size);

	}
	return bb.GetSize();
}

void CElement::LoadNested(CStyle * s, CXon * n, std::function<CElement *(CXon *, CElement *)> & load)
{
	for(auto i : n->Children)
	{
		auto c = load(i, null);
		AddNode(c);
		c->Free();
	}
}

void CElement::Load(CStyle * s, CString & u)
{
	CMap<CString, CProtocolConnection<IUwmProtocol>> classes;

	for(auto i : Level->Nexus->GetRegistry(L"World.Uwm"))
	{
		auto c = Level->Nexus->Connect<IUwmProtocol>(this, i.first->Url, UWM_PROTOCOL);

		for(auto j : i.second->Children)
		{
			classes[j->Get<CString>()] = c;
		}
	}

	auto rs = Level->Storage->OpenReadStream(u);
	auto & d = CTonDocument(CXonTextReader(rs));
	Level->Storage->Close(rs);

	std::function<CElement *(CXon *, CElement *)> load;

	load =	[this, &load, &classes, s](CXon * n, CElement * wn) -> CElement *
			{
				if(!wn)
				{
					wn = classes[n->Id]->CreateElement(n->Name, n->Id);
				}

				wn->LoadNested(s, n, load);

				if(n->Value && n->Value->As<CXon>())
				{
					if(auto sn = n->Value->As<CXon>()->One(L"Style"))
						if(auto xon = s->Document->One(sn->AsString()))
							wn->LoadProperties(s, xon);

					wn->LoadProperties(s, n->Value->As<CXon>());
				}
											   
				return wn;
			};

	load(d.Children.front(), this);
}

void CElement::ApplyStyles(CStyle * s, CList<CString> const & classes)
{
	for(auto i : classes)
	{
		LoadProperties(s, s->Document->One(i));
	}
}

CUnit * CElement::GetUnit()
{
	auto p = this;

	while(p && (!p->As<CModel>() || !p->As<CModel>()->Unit))
	{
		p = p->Parent;
	}

	return p ? p->As<CModel>()->Unit : null;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CElement::InPadding(CFloat3 & p)
{
	auto v = ((M.LF + B.LF <= p.x && p.x < M.LF + B.LF + P.LF) || (W - M.RT - B.RT - P.RT <= p.x && p.x < W - M.RT - B.RT)) && (M.BM + B.BM <= p.y && p.y < H - M.TP - B.TP);
	auto h = ((M.BM + B.BM <= p.y && p.y < M.BM + B.BM + P.BM) || (H - M.TP - B.TP - P.TP <= p.y && p.y < H - M.TP - B.TP)) && (M.LF + B.LF <= p.x && p.x < W - M.RT - B.RT);
	return v || h;
}

bool CElement::InBorder(CFloat3 & p)
{
	auto v = ((M.LF <= p.x && p.x < M.LF + B.LF) || (W - M.RT - B.RT <= p.x && p.x < W - M.RT)) && (M.BM <= p.y && p.y < H - M.TP);
	auto h = ((M.BM <= p.y && p.y < M.BM + B.BM) || (H - M.TP - B.TP <= p.y && p.y < H - M.TP)) && (M.LF <= p.x && p.x < W - M.RT);
	return v || h;
}

bool CElement::InMargin(CFloat3 & p)
{
	auto v = ((0 <= p.x && p.x < M.LF) || (W - M.RT <= p.x && p.x < W)) && (0 <= p.y && p.y < H);
	auto h = ((0 <= p.y && p.y < M.BM) || (H - M.TP <= p.y && p.y < H)) && (0 <= p.x && p.x < W);
	return v || h;
}

CString CElement::GetStatus()
{
	CString o;
	o += CString::Format(L"CW=%4g ", C.W);
	o += CString::Format(L"CH=%4g ", C.H);
	o += CString::Format(L"M=%s ", M.ToString());
	o += CString::Format(L"B=%s ", B.ToString());
	o += CString::Format(L"P=%s ", P.ToString());

	return o;
}

void * CElement::FindField(CString const & f)
{
	void * p = null;

	if(f == L"W")	p = &W; else
	if(f == L"H")	p = &H; else
	if(f == L"IW")	p = &IW; else
	if(f == L"IH")	p = &IH; else
	   
	if(f == L"M.LF")	p = &M.LF; else
	if(f == L"M.RT")	p = &M.RT; else
	if(f == L"M.TP")	p = &M.TP; else
	if(f == L"M.BM")	p = &M.BM; else
	if(f == L"M.FT")	p = &M.FT; else
	if(f == L"M.BK")	p = &M.BK; else
	   
	if(f == L"B.LF")	p = &B.LF; else
	if(f == L"B.RT")	p = &B.RT; else
	if(f == L"B.TP")	p = &B.TP; else
	if(f == L"B.BM")	p = &B.BM; else
	if(f == L"B.FT")	p = &B.FT; else
	if(f == L"B.BK")	p = &B.BK; else
	   
	if(f == L"P.LF")	p = &P.LF; else
	if(f == L"P.RT")	p = &P.RT; else
	if(f == L"P.TP")	p = &P.TP; else
	if(f == L"P.BM")	p = &P.BM; else
	if(f == L"P.FT")	p = &P.FT; else
	if(f == L"P.BK")	p = &P.BK; else

	if(f == L"M")	p = &M; else
	if(f == L"B")	p = &B; else
	if(f == L"P")	p = &P;

	else 
		throw CException(HERE, L"Unknown field");

	return p;
}

void CElement::Express(CString const & f, std::function<CSize(bool)> e)
{
	if(f == L"C")
		Expressions.Sizes[&C] = e;
	else
		throw CException(HERE, L"Unknown field");
}

void CElement::Express(CString const & f, std::function<float()> e)
{
	auto p = (float *)FindField(f);

	if(p == &W)
	{
		if(Expressions.Floats.Contains(&IW))
			Expressions.Floats.Remove(&IW);
	}
	else if(p == &IW)
	{
		if(Expressions.Floats.Contains(&W))
			Expressions.Floats.Remove(&W);
	}
	else if(p == &H)
	{
		if(Expressions.Floats.Contains(&IH))
			Expressions.Floats.Remove(&IH);
	}
	else if(p == &IH)
	{
		if(Expressions.Floats.Contains(&H))
			Expressions.Floats.Remove(&H);
	}

	Expressions.Floats[p] = e;
}
			
void CElement::Express(CString const & f, std::function<CFloat6()> e)
{
	auto p = (CFloat6 *)FindField(f);

	Expressions.Float6s[p] = e;
}

void CElement::Reset(CString const & f)
{
	auto p = FindField(f);
	
	if(Expressions.Floats.Contains((float *)p))
		Expressions.Floats.Remove((float *)p);

	if(Expressions.Float6s.Contains((CFloat6 *)p))
		Expressions.Float6s.Remove((CFloat6 *)p);
}

float CElement::WtoIW(float w)
{
	return w - (M.LF + B.LF + P.LF + M.RT + B.RT + P.RT);
}

float CElement::IWtoW(float iw)
{
	return iw + (M.LF + B.LF + P.LF + M.RT + B.RT + P.RT);
}

float CElement::HtoIH(float h)
{
	return h - (M.BM + B.BM + P.BM + M.TP + B.TP + P.TP);
}

float CElement::IHtoH(float ih)
{
	return ih + (M.BM + B.BM + P.BM + M.TP + B.TP + P.TP);
}

void CElement::LoadProperties(CStyle * s, CXon * n)
{
	for(auto i : n->Children)
	{
		auto t = i->Value ? i->AsString().Split(L";") : CArray<CString>();

		if(i->Name == L"W" || i->Name == L"H" /*|| i->Name == L"D" || i->Name == L"IW" || i->Name == L"IH" || i->Name == L"ID"*/)
		{
			CList<std::function<float(float)>> exps;
			float * pmax;
			float * smax;

			if(i->Name == L"W"){ smax = &Slimits.Smax.W; pmax = &Slimits.Pmax.W; } else 
			if(i->Name == L"H"){ smax = &Slimits.Smax.H; pmax = &Slimits.Pmax.H; }

			for(auto & e : t)
			{
				if(e.EndsWith(L"%"))
				{
					auto v = CFloat::Parse(e);
					exps.push_back([v, pmax, smax](float a){ return min(*smax, *pmax * v/100.f); });
				}
				else if(e.StartsWith(L">"))
				{
					CString a = e.data() + 1;
					if(a == L"CW")
					{
						exps.push_back([this](float a){ return isfinite(C.W) ? max(a, IWtoW(C.W)) : a; });
					}
					else if(a == L"CH")
					{
						exps.push_back([this](float a){ return isfinite(C.H) ? max(a, IHtoH(C.H)) : a; });
					}
					else
					{
						auto v = CFloat::Parse(a);
						exps.push_back([v](float a){ return max(a, v); });
					}
				}
				else
				{
					auto v = CFloat::Parse(e);
					exps.push_back([v](float a){ return v; });
				}
			}
			
			Express(i->Name,[exps, pmax]
							{
								float a = NAN;
								for(auto & i : exps)
								{
									a = i(a);
								}
								return a;
							});

		}
		else if(i->Name == L"M" || i->Name == L"B" || i->Name == L"P")
		{
			CFloat6 v(i->AsString());
			Express(i->Name, [v]{ return v; });
		}
		else if(i->Name.StartsWith(L"M.") || i->Name.StartsWith(L"B.") || i->Name.StartsWith(L"P."))
		{
			CFloat v(i->AsString());
			Express(i->Name, [v]{ return v; });
		}
	}
}


void CElement::UseCanvas(CVisual * v, CMesh * mesh, CShader * s)
{
	Texture = Level->Engine->TextureFactory->CreateTexture();

	auto material = new CMaterial(&Level->Engine->EngineLevel, s);
	material->Textures[L"DiffuseTexture"] = Texture;	
	material->Samplers[L"DiffuseSampler"].SetFilter(ETextureFilter::Point, ETextureFilter::Point, ETextureFilter::Point);
	material->Samplers[L"DiffuseSampler"].SetAddressMode(ETextureAddressMode::Clamp, ETextureAddressMode::Clamp);

	v->SetMaterial(material);
	
	material->Free();

	if(!mesh)
	{
		CanvasMesh = new CSolidRectangleMesh(&Level->Engine->EngineLevel);
		v->SetMesh(CanvasMesh);
		CanvasMesh->Free();
	}
}

void CElement::Draw()
{
}

void CElement::UpdateCanvas()
{
	if(Texture && IW > 1 && IH > 1 && Slimits && Climits)
	{
		auto w = ceil(IW);
		auto h = ceil(IH);

		auto tw = int(w * Level->Engine->DisplaySystem->Scaling.x);
		auto th = int(h * Level->Engine->DisplaySystem->Scaling.y);
		
		if(Texture->W != tw || Texture->H != th)
		{
			if(Texture->IsEmpty())
			{
				Texture->Create(tw, th, 0, 1, ETextureFeature::Canvas, CTexture::DefaultFormat);
			}
			else if(tw != Texture->W || th != Texture->H)
			{
				Texture->Resize(tw, th, 0, false);
			}

			if(CanvasMesh)
			{
				CanvasMesh->Generate(O.x, O.y, float(w), float(h));
			}
	
		}

		Draw();
	}
}




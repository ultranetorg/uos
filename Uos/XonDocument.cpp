#include "StdAfx.h"
#include "XonDocument.h"

using namespace uos;

CXonDocument::CXonDocument()
{
}

CXonDocument::CXonDocument(CXonDocument && d)
{
	Children = d.Children;
	d.Children.clear();

	Name = d.Name;

	Value = d.Value;
	d.Value = null;

	//Parent = d.Parent;
	//Templates = d.Templates; d.Templates.clear();
	//IsTemplate = false;
	//Value = null;
	//Removed;
	//IsRemoved = false;
}

CXonDocument::~CXonDocument()
{

}

void CXonDocument::Load(CXonDocument * t, IXonReader & r)
{
	if(!Children.empty())
	{
		throw CException(HERE, L"Must be empty");
	}
		
	auto e = r.Read();
	
	CXon * n = null;

	while(r.Current != EXonToken::End)
	{
		n = Load(r, this, t);
		r.ReadNext();

		if(n == null)
		{
			break;
		}
	}
}

CXon * CXonDocument::Load(IXonReader & r, CXon * parent, CXon * tparent)
{
	CXon * n = null;
	CXon * t = null;

	while(r.Current != EXonToken::End)
	{
		switch(r.Current)
		{
			case EXonToken::NodeBegin:
				n = new CXon();
				n->Parent = parent;
				break;

			case EXonToken::NodeEnd:
				if(t)
				{
					if(tparent)
					{
						auto d = tparent->Children.Find([n](auto j){  return j->Name == n->Name && j->Value && n->Value && n->Value->Equals(*j->Value); }); // from default list?
						if(d)
							t = d;
					}
						
					for(auto i : t->Children)
					{
						//if(!i->IsTemplatesOwner)
						{
							auto c = n->Children.Find([i](auto j){ return j->Name == i->Name; });
							if(!c)
							{
								n->Children.push_back(i->CloneInternal(n));
							}
						}
					}
				}
				return n;

			case EXonToken::NameBegin:
			{
				r.ReadName(n->Name, n->Id);

				auto pre = n->Name[0];

				if(pre == L'-')
				{
					n->Name = n->Name.substr(1);
					n->IsRemoved = true;
				}
				if(pre == L'*')
				{
					n->Name = n->Name.substr(1);
				}

				if(tparent)
				{
					t = tparent->Templates.Find([n](auto i){ return i->Name == n->Name; }); // multi merge
					if(!t)
						t = tparent->Children.Find([n](auto i){ return i->Name == n->Name; }); // single merge
				}
				else if(parent) // self merge
				{
					t = parent->Templates.Find([n](auto i){ return i->Name == n->Name; });
				}
					
				if(t)
				{
					//delete n;
					//n = t->CloneInternal(parent);
					//parent->Children.push_back(n);
					parent->Children.push_back(n);
				}
				else
				{
					if(pre == L'*') // this is template
					{
						parent->IsTemplatesOwner = true;
						parent->Templates.push_back(n);
					}
					else
						parent->Children.push_back(n); // to children
				}

				if(t && t->Value)
					n->Set(t->Value);

				break;
			}	

			case EXonToken::SimpleValueBegin:
				n->GiveValue();
				r.ReadValue(n->Value->As<CXonSimpleValue>());
				break;

			case EXonToken::AttrValueBegin:
			{
				auto a = new CXon();
				a->Value = parent->GetRoot()->Value->Clone();
				delete n->Value;
				n->Value = a;
		
				while(r.ReadNext() == EXonToken::NodeBegin)
				{
					Load(r, a, null);

					//if(r.Current == EXonToken::AttrValueEnd)
					//{
					//	break;;
					//}
				}
				break;
			}
			case EXonToken::ChildrenBegin:
				while(r.ReadNext() == EXonToken::NodeBegin)
				{
					if(Load(r, n, t) == null)
					{
						return n;
					}
				}
				//while(auto i = p->Children.Find([](auto j){ return j->IsRemoved; }))
				//{
				//	p->Children.remove(i);
				//	p->Removed.push_back(i);
				//}
				break;
		}
			
		r.ReadNext();
	}
	return n;
}
	
void CXonDocument::Merge(CXon * dnode, CXon * cnode)
{
	if(cnode->Value)
	{
		dnode->Set(cnode->Value);
	}

	for(auto i : cnode->Children)
	{
		if(i->IsRemoved) // remove defaults
		{
			auto d = dnode->Children.Find([i](auto j){ return i->Name == j->Name && j->Value && i->Value->Equals(*j->Value); });

			dnode->Children.remove(d);
			dnode->Removed.push_back(d);
			continue;
		}

		bool clone = false;

		auto t = dnode->Templates.Find([i](auto j){ return i->Name == j->Name; }); // multi?

		if(t)
		{
			auto d = dnode->Children.Find([i](auto j){ return i->Name == j->Name && j->Value && i->Value->Equals(*j->Value); });
			if(d) // default list item?
			{
				t = d;
			}
			else // use template
			{
				t = t->CloneInternal(dnode);
				clone = true;
			}
		}
		else  // single
			t = dnode->Children.Find([i](auto j){ return i->Name == j->Name; });
			
		Merge(t, i);

		if(clone)
			dnode->Children.push_back(t);
	}
}

CString CXonDocument::Dump()
{
	CString s;
		
	CList<CXon*> a;
	a.push_back(this);

	auto maxnamelen = a.GetHeirarchicalMax([](CXon * i){ return &i->Children; },[](auto i){ return i->Name.size(); });
	auto maxdfnlen = a.GetHeirarchicalMax([](CXon * i){ return &i->Children; },	[](auto i)
																						{
																							CString s;
																							for(auto j : i->Templates)
																							{
																								s += j->Name + L" ";
																							}
																							return s.size(); 
																						});

	std::function<void(CXon *, const CString & s)> dumpNode = [this, &dumpNode, &s, maxnamelen, maxdfnlen](CXon * n, const CString & t)
	{
		s += CString::Format(CString::Format(L"%%-%zus", maxnamelen), t + n->Name);

		if(n->Value)
		{
			s += L" ";
			s += dynamic_cast<CXonTextValue *>(n->Value)->Text;
		}
				
		s+= L"\r\n";

		for(auto i : n->Templates)
		{
			dumpNode(i, t + L"  +");
		}

		for(auto i : n->Children)
		{
			dumpNode(i, t + L"  ");
		}
	};

	dumpNode(this, L"");

	return s;
}

void CXonDocument::Save(IXonWriter * w, CXonDocument * d)
{
	if(d)
	{
		auto t = CreateDifference(this, d);

		if(t)
		{
			w->Write(t);
			delete t;
		}
	}
	else
	{
		w->Write(this);
	}
}

CXon * CXonDocument::CreateDifference(CXon * p, CXon * d)
{
	CXon * n = null;
	if(!p->Value != !d->Value || (p->Value && d->Value && !(p->Value->Equals(*d->Value))))
	{
		n = new CXon(p->Name);
		n->Set(p->Value);
	}

	for(auto i : p->Removed)
	{
		auto pd = p->Children.Find([i](auto j){ return i->Name == j->Name && j->Value && i->Value->Equals(*j->Value); }); // have similar in children?
		auto dd = d->Children.Find([i](auto j){ return i->Name == j->Name && j->Value && i->Value->Equals(*j->Value); }); // is item of default list?
		if(!pd && dd)
		{
			if(!n)
				n = new CXon(p->Name);
			auto r = n->Add(i->Name);
			r->Set(i->Value);
			r->IsDifferenceDeleted = true;
		}
	}

	for(auto i : p->Children)
	{
		auto t = d->Templates.Find([i](auto j){ return i->Name == j->Name; });
		bool multi = t != null;

		if(t)
		{
			if(!i->Value)
			{
				throw CException(HERE, L"Multi node requires both key and value");
			}
				
			auto dd = d->Children.Find([i](auto j){ return i->Name == j->Name && j->Value && i->Value->Equals(*j->Value); });
			if(dd)
			{
				t = dd;
			}
		}
		else
			t = d->Children.Find([i](auto j){ return i->Name == j->Name; });

		auto diff = CreateDifference(i, t);
		if(diff)
		{
			if(multi)
				diff->Set(i->Value);

			if(!n)
				n = new CXon(p->Name);

			n->Add(diff);
		}
	}

	return n;
}

//CBinaryNode * CXonDocument::CreateBinary(CXon * p)
//{
//	return CreateBinary(p, this);
//}
//		
//CBinaryNode * CXonDocument::CreateBinary(CXon * target, CXon * p)
//{
///*
//	if(p->HasAnyCustom() || // все кастомные параметры - сохраняются
//		(p->HasAnyAssigned() && (p == target || p->IsAncestor(target) || target->IsAncestor(p)))) // новоназначеные параметры сохрнаются только, если они являются частью ветки целевого параметра
//	{
//*/
//
//		auto n = new CBinaryNode(p->Name);
//		if(p->Value != null)
//		{
//			n->Value = p->Value->ToBinary();
//			n->Type = p->Value->GetTypeName();
//		}
//
//		for(auto i : p->Many())
//		{
//			auto c = CreateBinary(target, i);
//			if(c != null)
//			{
//				n->Add(c);
//			}
//		}
//
//		return n;
//	//}
//	//return null;
//}
//
//
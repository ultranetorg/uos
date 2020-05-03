#include "StdAfx.h"
#include "XonWriter.h"

using namespace std;
using namespace uos;

CXonTextWriter::CXonTextWriter()
{
	IsWriteTypes = false;
}

void CXonTextWriter::Write(CXon * root)
{
	if(root->Parent)
		throw CException(HERE, L"Must be root");

	std::wostringstream s;
	
	for(auto i : root->Children)
	{
		Write(s, i, 0);
	}

	if(OutStream)
	{
		auto u = CString(s.str()).ToUtf8();
		OutStream->Write("\xEF\xBB\xBF", 3);
		OutStream->Write(u.data(), u.size());
	}
}

void CXonTextWriter::Write(std::wostringstream & s, CXon * n, int d)
{
	CString t(d, L'\t');

	s << t << (n->IsDifferenceDeleted ? L"-" : L"") << n->Name;
	if(IsWriteTypes && !n->Id.empty())
	{
		s << L":" << n->Id;
	}
	if(n->Value)
	{
		CString v = dynamic_cast<CXonTextValue *>(n->Value)->Text;

		if(!v.empty())
		{
			auto q = v.find_first_of(L" \t\r\n{}") != std::string::npos || v.find(L"//") != std::string::npos;
			auto qq = v.find_first_of(L'\'') != std::wstring::npos;
	
			s << L" = ";
			if(q || qq)
			{
				s << L"'";
			}
	
			if(qq)
				v = v.Replace(L"'", L"''");
	
			s << v;
	
			if(q || qq)
			{
				s << L"'";
			}
		}
		else
			s <<  L" = ''";
	}

	s << endl;

	if(!n->Children.empty())
	{
		s << t << L"{" << endl;
		for(auto i : n->Children)
		{
			Write(s, i, d+1);
		}
		s << t << L"}" << endl;
	}
}

/////////////////////////////// CXonBinaryWriter ///////////////////////////////////////////////

CXonBinaryWriter::CXonBinaryWriter(CStream * s)
{
	Stream = s;
}

void CXonBinaryWriter::Write(CXon * root)
{
	if(root->Parent)
		throw CException(HERE, L"Must be root");

	CAnsiString sig = "XON1";
	Stream->Write(sig.data(), sig.size());

	CArray<CString> indexes;
	CMap<CString, char> types;

	std::function<void(CXon * p)> collectTypes =[this, &collectTypes, &types, &indexes](auto p)
												{
													CString t = p->Id;
													if(t == CString::TypeName)
													{
														t = L"utf16.array";
													}
													if(t != L"" && !types.Contains(t))
													{
														if(types.size() > 256)
														{
															throw CException(HERE, L"types.size() > 256 not supported");
														}

														types[t] = (char)types.size();
														indexes.push_back(t);
													}
													for(auto i : p->Children)
													{
														collectTypes(i);
													}
												};

	collectTypes(root);

		
	auto n = types.size();
	Stream->Write(&n, sizeof(int));

	for(unsigned int i=0; i<indexes.size(); i++)
	{
		Stream->Write(indexes[i].data(), indexes[i].size()*2 + 2);
	}

	std::function<void(CXon *, CXon *)> save = [this, &types, &save](auto p, auto last)
	{
		char f = (unsigned char)EBonHeader::Null;
		if(!p->Children.empty())
		{
			f |= (char)EBonHeader::Parent;
		}
		
		if(p == last)
		{
			f |= (char)EBonHeader::Last;
		}

		auto t = p->Id;
		if(t == CString::TypeName)
		{
			t = L"utf16.array";
		}
					
		if(!t.empty())
		{
			f |= (char)EBonHeader::HasType;
		}

		auto v = dynamic_cast<CXonBinaryValue *>(p->Value);
		if(v)
		{
			f |= (char)EBonHeader::HasValue;

			auto s = v->Data.GetSize();

			if(s <= 8)
			{
				f |= (char)s-1;
			} 
			else
			{
				f |= (char)EBonHeader::BigValue;

				if(s < 1ll<<8)	f |= (0b00); else
				if(s < 1ll<<16)	f |= (0b01); else
				if(s < 1ll<<32)	f |= (0b10); else
								f |= (0b11);
			}
		}
						
		Stream->Write(&f, sizeof(EBonHeader));
		Stream->Write(p->Name.data(),	(int)p->Name.size()*2 + 2);

		if(!t.empty())
		{
			auto type = types.find(t);
			Stream->Write(&type->second, sizeof(type->second));
		}

		if(v)
		{
			auto s = v->Data.GetSize();	

			if(s > 8)
			{
				if(s < 1ll<<8)	Stream->Write(&(unsigned char)	s, 1); else
				if(s < 1ll<<16)	Stream->Write(&(unsigned short)	s, 2); else
				if(s < 1ll<<32)	Stream->Write(&(unsigned int)	s, 4); else
								Stream->Write(&s,				   8); 
			}
		
			Stream->Write(v->Data);
		}

		for(auto i : p->Children)
		{
			save(i, p->Children.back());
		}
	};

	for(auto i : root->Children)
	{
		save(i, root->Children.back());
	}
}



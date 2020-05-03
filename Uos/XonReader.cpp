#include "StdAfx.h"
#include "XonReader.h"

using namespace std;
using namespace uos;

CXonTextReader::CXonTextReader(CStream * stream)
{
	auto b = stream->Read();

	auto d = (unsigned char *)b.GetData();

	assert(b.GetSize() < INT_MAX);

	if(b.GetSize() >= 3 && d[0] == 0xEF && d[1] == 0xBB && d[2] == 0xBF) // utf-8
	{
		Text = CString::FromUtf8((char *)(d+3), (int)b.GetSize()-3);
	}
	else
	{
		Text = CString::FromUtf8((char *)d, (int)b.GetSize());
	}
}

CXonTextReader::CXonTextReader(CString const & t)
{
	Text = t;
}

CXonTextReader::~CXonTextReader()
{
}

EXonToken CXonTextReader::Read()
{
	if(!Text.empty())
	{
		Type.push_back(false);
		Current = EXonToken::NodeBegin;
		C = (wchar_t *)Text.c_str();
	}
	else
		Current = EXonToken::End;
	return Current;
}

EXonToken CXonTextReader::ReadNext()
{
	Next(C);
	
	if(*C)
	{
		switch(Current)
		{
			case EXonToken::NodeEnd:
			{
				if(*C == L'}') // after last child
				{
					bool t = Type.back();
					Type.pop_back();

					Current = t ? EXonToken::AttrValueEnd : EXonToken::ChildrenEnd;
					C++;
				}
				else // next child
				{
					Current = EXonToken::NodeBegin;
				}
				break;
			}
			case EXonToken::NodeBegin:
				Current = EXonToken::NameBegin;
				break;

			case EXonToken::NameEnd:
				if(*C == L'=')
				{
					C++;
					Current = EXonToken::ValueBegin;
				}
				else if(*C == L'{')
				{
					C++;
					Type.push_back(false);
					Current = EXonToken::ChildrenBegin;
				}
				else
					Current = EXonToken::NodeEnd;
				break;

			case EXonToken::ValueBegin:
				if(*C == L'{')
				{
					C++;
					Type.push_back(true);
					Current = EXonToken::AttrValueBegin;
				}
				else
					Current = EXonToken::SimpleValueBegin;
				break;
			
			case EXonToken::AttrValueBegin:
				if(*C == L'}')
				{
					C++;
					Current = EXonToken::AttrValueEnd;
				}
				else
				{
					Current = EXonToken::NodeBegin;
				}
				break;
			
			case EXonToken::ChildrenBegin:
				if(*C == L'}')
				{
					Current = EXonToken::ChildrenEnd;
					C++;
				}
				else
				{	
					Current = EXonToken::NodeBegin;
				}
				break;

			case EXonToken::ChildrenEnd:
				Current = EXonToken::NodeEnd;
				break;

			case EXonToken::AttrValueEnd:
			case EXonToken::SimpleValueEnd:
				Current = EXonToken::ValueEnd;
				break;

			case EXonToken::ValueEnd:
			{
				if(*C == L'{')
				{
					C++;
					Type.push_back(false);
					Current = EXonToken::ChildrenBegin;
				}
				else
					Current = EXonToken::NodeEnd;
				break;
			}
		}
	}
	else if(Current == EXonToken::ChildrenEnd)
	{
		Current = EXonToken::NodeEnd;
	}
	else
		Current = EXonToken::End;

	return Current;
}

void CXonTextReader::Next(wchar_t *& c)
{
	while(*c != 0)
	{
		if(*c == L' ' || *c == L'\t' || *c == L'\r' || *c == L'\n')
		{
			c++;
		}
		else if(*c == '/')
		{
			if(*(c+1) != 0 && *(c+1) == '/')
			{	
				c++;
				while(*c != 0 && *c != '\r' && *c != '\n')
					c++;
			}
			else
				break;
		}
		else
			break;
	}
}

void CXonTextReader::ReadName(CString & name, CString & type)
{
	bool typefound = false;
	bool q = false;

	while(true)
	{
		if(!q)
		{
			if(*C == 0 || *C == L' ' || *C == L'\t' || *C == L'\r' || *C == L'\n' || *C == L'{' || *C == L'}' || *C == L'=')
			{
				Current = EXonToken::NameEnd;
				return;
			}
			else if(*C == L'\'') // opening '
			{	
				//if(!foundsemicolon)
				q = true;
			}
			else if(*C == L':')
			{
				typefound = true;
			}
			else 
				if(!typefound)
				{
					name += *C;
				} 
				else
				{
					type += *C;
				}
		} 
		else
		{
			if(*C == '\'') // closing ' or escaping
			{
				C++;
				if(*C == '\'')
				{
					name += *C;
					//c++;
				}
				else
				{
					Current = EXonToken::NameEnd;
					break;
				}
			}
			else
				name += *C;
		}
		C++;
	}
}


void CXonTextReader::ReadValue(CXonSimpleValue * v)
{
	CString value;

	bool found = false;
	bool q = false;

	while(*C != 0)
	{
		if(!q)
		{
			if(*C == L' ' || *C == L'\t' || *C == L'\r' || *C == L'\n' || *C == L'}' || *C == L'{')
			{
				if(found)
					break;
			}
			else if(*C == L'\'') // opening '
			{	
				//if(!foundsemicolon)
				q = true;
			}
			else
			{
				if(!found)
					found = true;
				value += *C;
			}
		} 
		else
		{
			if(*C == '\'') // closing ' or escaping
			{
				C++;
				if(*C == '\'')
				{
					value += *C;
					//c++;
				}
				else
					break;
			}
			else
				value += *C;

		}
		C++;
	}

	v->As<CXonTextValue>()->Text = value;
		
	Current = EXonToken::SimpleValueEnd;
		
}

///////////////////////// CXonBinaryReader /////////////////////////////////////////

CXonBinaryReader::CXonBinaryReader(CStream * s)
{
	Stream = s;
}

CXonBinaryReader::~CXonBinaryReader()
{

}

EXonToken CXonBinaryReader::Read()
{
	if(Stream->GetSize() > 0)
		Current = EXonToken::NodeBegin;
	else
		Current = EXonToken::End;

	CAnsiString sig;
	sig.resize(4);
	Stream->Read((void *)sig.data(), sig.size());

	if(sig == "XON1")
	{
		int n;
		Stream->Read(&n, sizeof(n));

		Types.resize(n);

		for(int i=0; i<n; i++)
		{
			wchar_t c;
			CString name;
			Stream->Read(&c, 2);
			while(c != '\0')
			{	
				name += c;
				Stream->Read(&c, 2);
			}
			Types[i] = name;
		}

		return Current;
	}
	else
		Current = EXonToken::End;

	return Current;
}

EXonToken CXonBinaryReader::ReadNext()
{
	if(!Stream->IsEnd())
	{
		switch(Current)
		{
			case EXonToken::NodeBegin:
				unsigned char f;
				Stream->Read(&f, sizeof(f));
				Flags.push_back(f);

				Current = EXonToken::NameBegin;
				break;

			case EXonToken::NodeEnd:
				if(Flags.empty()) // if last of roots
				{
					return EXonToken::End;
				}

				if(Flags.back() & (unsigned char)EBonHeader::Last)
					Current = EXonToken::ChildrenEnd;
				else
					Current = EXonToken::NodeBegin;

				Flags.pop_back();
				break;

			case EXonToken::NameEnd:
			{
				//auto index = (0b0011'1111 & Flags.back());
				
				if(Flags.back() & (unsigned char)EBonHeader::HasValue)
					Current = EXonToken::SimpleValueBegin;
				else if(Flags.back() & (unsigned char)EBonHeader::Parent)
					Current = EXonToken::ChildrenBegin;
				else
					Current = EXonToken::NodeEnd;
				break;
			}
			case EXonToken::ChildrenBegin:
				Current = EXonToken::NodeBegin;
				break;

			case EXonToken::ChildrenEnd:
				Current = EXonToken::NodeEnd;
				break;

			case EXonToken::ValueBegin:
				Current = EXonToken::SimpleValueBegin;
				break;

			case EXonToken::SimpleValueEnd:
				Current = EXonToken::ValueEnd;
				break;

			case EXonToken::ValueEnd:
			{
				if(Flags.back() & (unsigned char)EBonHeader::Parent)
					Current = EXonToken::ChildrenBegin;
				else
					Current = EXonToken::NodeEnd;
				break;
			}
		}
	}
	else
		Current = EXonToken::End;

	return Current;
}

void CXonBinaryReader::ReadName(CString & name, CString & type)
{
	wchar_t c;
	
	Stream->Read(&c, 2);
	while(c != '\0')
	{	
		name += c;
		Stream->Read(&c, 2);
	}

	if(Flags.back() & (unsigned char)EBonHeader::HasType)
	{
		char t;
		Stream->Read(&t, 1);
		type = Types[t];
	}
	
	if(type == L"utf16.array")
	{	
		type = CString().GetTypeName();
	}

	Current = EXonToken::NameEnd;
}

void CXonBinaryReader::ReadValue(CXonSimpleValue * v)
{
	int64_t s=0;

	if((Flags.back() & (unsigned char)EBonHeader::BigValue) == 0)
	{
		s = (Flags.back() & 0b0000'0111) + 1;
	}
	else
	{
		auto f = Flags.back() & 0b0000'0011;
		if(f == 0b00)	Stream->Read(&s, 1); else 
		if(f == 0b01)	Stream->Read(&s, 2); else 
		if(f == 0b10)	Stream->Read(&s, 4); else 
		if(f == 0b11)	Stream->Read(&s, 8);
	}

	dynamic_cast<CXonBinaryValue *>(v)->Data = Stream->Read(s);

	Current = EXonToken::SimpleValueEnd;
}

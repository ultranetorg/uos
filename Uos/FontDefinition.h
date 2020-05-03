#pragma once

namespace uos
{
	class UOS_LINKING CFontDefinition : public ISerializable
	{
		public:
			CString		Family;
			float		Size = NAN;
			bool		Bold;
			bool		Italic;
		
		
			bool operator != (const CFontDefinition & v) const
			{
				if(Family != v.Family)
					return true;
				if(Size != v.Size)
					return true;
				if(Bold != v.Bold)
					return true;
				if(Italic != v.Italic)
					return true;
				return false;
			}

			std::wstring CFontDefinition::GetTypeName()
			{
				return L"font";
			}
			
			void CFontDefinition::Read(CStream * s)
			{
				throw CException(HERE, L"Not implemented");
			}

			int64_t CFontDefinition::Write(CStream * s)  
			{
				throw CException(HERE, L"Not implemented");
			}

			void CFontDefinition::Write(std::wstring & s)
			{
				s += CString::Format(L"%s,%f,%s,%s", Family, Size, Bold ? L"bold" : L"", Italic ? L"italic" : L"");
			}

			void CFontDefinition::Read(const std::wstring & v)
			{
				auto p = CString(v).Split(L",");
				if(p.size() > 0)
				{
					Family = p[0];
				}
				if(p.size() > 1)
				{
					Size = CFloat::Parse(p[1]);
				}

				Bold = p.Has([](auto & i){ return i.EqualsInsensitive(L"bold"); });
				Italic = p.Has([](auto & i){ return i.EqualsInsensitive(L"italic"); });
	
				if(Family.empty() || !isfinite(Size))
				{
					throw CException(HERE, CString::Format(L"Invalid font description: %s", v));
				}
			}

			ISerializable * CFontDefinition::Clone()
			{
				auto a = new CFontDefinition();

				a->Family	= Family;;
				a->Size	= Size;
				a->Bold	= Bold;
				a->Italic	= Italic;

				return a;
			}

			virtual bool Equals(const ISerializable & a) const
			{
				return !(this->operator!=(dynamic_cast<const CFontDefinition &>(a)));
			}

	};
}

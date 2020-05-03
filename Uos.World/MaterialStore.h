#pragma once

namespace uos
{
	class CMaterialStore : public IMaterialStore
	{
		public:
			CRefList<CMaterial *>						Materials;
			CRefList<CShader *>							Shaders;
			CRefList<CTexture *>						Textures;
			CWorldLevel *								Level;


			CMaterialStore(CWorldLevel * l)
			{
				Level = l;
			}

			~CMaterialStore()
			{
			}

			CString Add(CMaterial * m)
			{
				if(!Materials.Contains(m))
				{
					if(!Shaders.Contains(m->Shader))
					{
						Shaders.Add(m->Shader);
					}

					Materials.Add(m);

					for(auto & i : m->Textures)
					{
						if(!Textures.Contains(i.second.Texture))
						{
							Textures.Add(i.second.Texture);
						}
					}
				}

				return m->Name;
			}

			void Save(CXon * r)
			{
				for(auto i : Materials)
				{
					i->Save(r->Add(L"Material"), this);
				}

				for(auto i : Shaders)
				{
					i->Save(r->Add(L"Shader"));
				}

				for(auto i : Textures)
				{
					auto t = r->Add(L"Texture");
					i->Save(t);
				}
			}
						
			void Load(CXon * p)
			{
				for(auto i : p->Many(L"Texture"))
				{
					auto t = Level->Engine->TextureFactory->CreateTexture();
					t->Load(i);
					Textures.AddNew(t);
				}
				for(auto i : p->Many(L"Shader"))
				{
					auto t = new CShader(L"");
					t->Load(i);
					Shaders.AddNew(t);
				}
				for(auto i : p->Many(L"Material"))
				{
					auto t = new CMaterial(&Level->Engine->EngineLevel, GetShader(i->Get<CString>(L"Shader")));
					t->Load(i, this);
					Materials.AddNew(t);
				}
			}

			CTexture * GetTexture(const CString & name)
			{
				return Textures.Find([name](auto & i){ return i->Name == name; });
			}
			
			CMaterial * GetMaterial(const CString & name) override
			{
				return Materials.Find([name](auto & i){ return i->Name == name; });
			}

			CShader * GetShader(const CString & name) override
			{
				return Shaders.Find([name](auto & i){ return i->Name == name; });
			}
	};
}

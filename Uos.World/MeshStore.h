#pragma once

namespace uos
{
	class CMeshStore : public IMeshStore
	{
		public:
			CMap<CMesh *, CString>	Meshes;
			CWorldLevel *				Level;

			CString Add(CMesh * t)
			{
				auto i = Meshes.find(t);
				if(i == Meshes.end())
				{
					Meshes[t] = CString::Format(L"Mesh-%04d", Meshes.size());
					t->Take();
				}

				return Meshes[t];
			}

			void Save(CXon * r)
			{
				for(auto i : Meshes)
				{
					auto p = r->Add(L"Mesh");
					p->Set(i.second);
					i.first->Save(p);
				}
			}

			void Load(CXon * p)
			{
				for(auto i : p->Many(L"Mesh"))
				{
					auto t = Level->Engine->CreateMesh();
					t->Load(i);
					Meshes[t] = i->AsString();
				}
			}

			CMesh * Get(const CString & name)
			{
				return std::find_if(Meshes.begin(), Meshes.end(), [name](auto & i){ return i.second == name; })->first;
			}
			
			CMeshStore(CWorldLevel * l)
			{
				Level = l;
			}

			~CMeshStore()
			{
				for(auto i : Meshes)
				{
					i.first->Free();
				}
			}
	};
}

#pragma once

namespace uos
{
	class UOS_WORLD_LINKING CLogo : public CModel
	{
		public:
			CWorld * 									Level;

			UOS_RTTI
			CLogo(CWorld * l) : CModel(l, l->Server, ELifespan::Visibility, GetClassName())
			{
				Level = l;
				Tags = {AREA_SERVICE_FRONT};

				auto t = Level->Engine->TextureFactory->CreateTexture();
				t->Load(Level->Server->Info->HInstance, IDB_UOS_PNG);

				auto mtl = new CMaterial(&Level->Engine->EngineLevel, Level->Engine->PipelineFactory->DiffuseTextureShader);
				mtl->AlphaBlending = true;
				mtl->Textures[L"DiffuseTexture"] = t;	
				mtl->Samplers[L"DiffuseSampler"].SetFilter(ETextureFilter::Point, ETextureFilter::Point, ETextureFilter::Point);


				auto msh = new CSolidRectangleMesh(&Level->Engine->EngineLevel);
				msh->Generate(0, 0, float(t->W), float(t->H));
				Visual->SetMaterial(mtl);
				Visual->SetMesh(msh);

				Size = CSize(float(t->W), float(t->H), 0);

				t->Free();
				msh->Free();
				mtl->Free();
			}

			~CLogo()
			{
			}

			virtual void DetermineSize(CSize & smax, CSize & s) override
			{
				if(smax.W < Size.W)
				{
					auto a = smax.W/Size.W;
					Size = Size * a * 0.8f;
					Visual->Mesh->As<CSolidRectangleMesh>()->Generate(0, 0, Size.W, Size.H);
				}
			}

			virtual CTransformation DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t) override
			{
				return CTransformation(-Size.W/2, -Size.H/2, Unit->GetActualView()->PrimaryCamera->UnprojectZ(1, 1));;
			}
	};
}

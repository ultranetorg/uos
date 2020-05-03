#pragma once

namespace uos
{
	class UOS_WORLD_LINKING CSphere : public CModel
	{
		public:
			CWorld * 									Level;

			UOS_RTTI
			CSphere(CWorld * l) : CModel(l, l->Server, ELifespan::Session, GetClassName())
			{
				Level = l;

				Visual->Enable(false);

				auto m = new CSphereMesh(&l->Engine->EngineLevel);
				//w = MainPerspView->PrimaryCamera->GetX(1000000, MainViewport->Width/2);
				//h = MainPerspView->PrimaryCamera->GetY(1000000, MainViewport->Height/2);
				m->Generate(0, 0, 0, 1e6, 16);
				
				Transform(0, 0, 0);
				Active->Listen(true);
				Active->SetMesh(m);
				Active->Clipping = EClipping::No;

				m->Free();
			}

			~CSphere()
			{
			}

			virtual void DetermineSize(CSize & smax, CSize & s) override
			{
				//if(ms.W < Size.W)
				//{
				//	auto a = ms.W/Size.W;
				//	Size = Size * a * 0.8f;
				//	Visual->Mesh->As<CsectMesh>()->Generate(0, 0, 0, Size.W, Size.H);
				//}
			}

			virtual CTransformation DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t) override
			{
				return Transformation;
			}
	};
}

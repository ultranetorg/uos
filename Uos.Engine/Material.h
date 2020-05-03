#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "IMaterialStore.h"
#include "ConstantBuffer.h"
#include "Shader.h"

namespace uos
{
	enum class ETextureAddressMode
	{
		Wrap, Clamp, Mirror
	};

	enum class ETextureFilter
	{
		Linear, Point, Anisotropic
	};

	static D3D11_TEXTURE_ADDRESS_MODE ToDx(ETextureAddressMode m)
	{
		if(m == ETextureAddressMode::Wrap)		return D3D11_TEXTURE_ADDRESS_WRAP; else
		if(m == ETextureAddressMode::Clamp)		return D3D11_TEXTURE_ADDRESS_CLAMP; else
		if(m == ETextureAddressMode::Mirror)	return D3D11_TEXTURE_ADDRESS_MIRROR; else
		
		throw CException(HERE, L"Not implemented");
	}

	static D3D11_FILTER ToDx(ETextureFilter min, ETextureFilter mag, ETextureFilter mip)
	{
		if(min == ETextureFilter::Linear && mag == ETextureFilter::Linear && mip == ETextureFilter::Linear)	return D3D11_FILTER_MIN_MAG_MIP_LINEAR; else
		if(min == ETextureFilter::Linear && mag == ETextureFilter::Linear && mip == ETextureFilter::Point)	return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; else
		if(min == ETextureFilter::Linear && mag == ETextureFilter::Point && mip == ETextureFilter::Linear)	return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR; else
		if(min == ETextureFilter::Linear && mag == ETextureFilter::Point && mip == ETextureFilter::Point)	return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT; else

		if(min == ETextureFilter::Point && mag == ETextureFilter::Linear && mip == ETextureFilter::Linear)	return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR; else
		if(min == ETextureFilter::Point && mag == ETextureFilter::Linear && mip == ETextureFilter::Point)	return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; else
		if(min == ETextureFilter::Point && mag == ETextureFilter::Point && mip == ETextureFilter::Linear)	return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR; else
		if(min == ETextureFilter::Point && mag == ETextureFilter::Point && mip == ETextureFilter::Point)	return D3D11_FILTER_MIN_MAG_MIP_POINT; else

		if(min == ETextureFilter::Anisotropic && mag == ETextureFilter::Anisotropic && mip == ETextureFilter::Anisotropic)	return D3D11_FILTER_ANISOTROPIC; else

		throw CException(HERE, L"Not implemented");
	}

	struct CSamplerBinding
	{
		ETextureAddressMode								UMode = ETextureAddressMode::Wrap;
		ETextureAddressMode								VMode = ETextureAddressMode::Wrap;
		ETextureAddressMode								WMode = ETextureAddressMode::Wrap;
		ETextureFilter									MinFilter = ETextureFilter::Linear;
		ETextureFilter									MagFilter = ETextureFilter::Linear;
		ETextureFilter									MipFilter = ETextureFilter::Linear;

		int												Slot = -1;

		D3D11_SAMPLER_DESC								DxDesc = {};
		CMap<CDirectDevice *, ID3D11SamplerState *>	 	DxSamplers;

		CSamplerBinding()
		{
			DxDesc.AddressU = ToDx(UMode);
			DxDesc.AddressV = ToDx(VMode);
			DxDesc.AddressW = ToDx(WMode);

			DxDesc.Filter = ToDx(MinFilter, MagFilter, MipFilter);

			DxDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			DxDesc.MinLOD = 0.0f;
			DxDesc.MaxLOD = D3D11_FLOAT32_MAX;
		}

		~CSamplerBinding()
		{
			for(auto & i : DxSamplers)
				i.second->Release();
		}

		void SetAddressMode(ETextureAddressMode u, ETextureAddressMode v)
		{
			UMode = u;
			VMode = v;

			DxDesc.AddressU = ToDx(u);
			DxDesc.AddressV = ToDx(v);
		}

		void SetFilter(ETextureFilter min, ETextureFilter max, ETextureFilter mip)
		{
			MinFilter = min;
			MagFilter = max;
			MipFilter = mip;

			DxDesc.Filter = ToDx(min, max, mip);
		}

		void Apply(CDirectDevice * d)
		{
			auto & s = DxSamplers[d];

			if(!s)
			{
				d->DxDevice->CreateSamplerState(&DxDesc, &s);
			}

			d->DxContext->PSSetSamplers(Slot, 1, &s);
		}

		void Load(CXon * p)
		{
			auto min = (ETextureFilter)p->Get<CInt32>(L"MinFilter").Value;
			auto mag = (ETextureFilter)p->Get<CInt32>(L"MagFilter").Value;
			auto mip = (ETextureFilter)p->Get<CInt32>(L"MipFilter").Value;
			SetFilter(min, mag, mip);

			if(auto a = p->One(L"UMode"))
			{
				UMode = (ETextureAddressMode)a->Get<CInt32>().Value;
			}
			if(auto a = p->One(L"VMode"))
			{
				VMode = (ETextureAddressMode)a->Get<CInt32>().Value;
			}
		}


		void Save(CXon * p)
		{
			p->Add(L"MinFilter")->Set((int)MinFilter);
			p->Add(L"MagFilter")->Set((int)MagFilter);
			p->Add(L"MipFilter")->Set((int)MipFilter);

			if(UMode != ETextureAddressMode::Wrap)
			{
				p->Add(L"UMode")->Set(int(UMode));
			}
			if(VMode != ETextureAddressMode::Wrap)
			{
				p->Add(L"VMode")->Set(int(VMode));
			}
		}
	};

	struct CTextureBinding
	{
		CTexture *		Texture = null;
		int				Slot = -1;

		CTextureBinding(){}
		~CTextureBinding(){ if(Texture) Texture->Free();  }
		CTextureBinding(CTexture * t) { Texture = sh_assign(Texture, t); }
		CTextureBinding(const CTextureBinding & s){ Texture = sh_assign(Texture, s.Texture); };
		CTextureBinding(CTextureBinding && s) : Texture(s.Texture){ s.Texture = null;  }
		CTextureBinding & operator = (CTextureBinding & s){ Texture = sh_assign(Texture, s.Texture);  return *this; };
		CTextureBinding & operator = (CTexture * t){ Texture = sh_assign(Texture, t); return *this; };

	};

	template<typename T> struct CConstantBinding
	{
		T						Value;
		int						Buffer = -1;
		int						Slot = -1;
		bool					Changed = true;

		CConstantBinding(){}
		CConstantBinding(const T & v) : Value(v){}
		T & operator = (const T & v) { 
			Value = v; Changed = true; return Value; 
		}
	};

	class UOS_ENGINE_LINKING CMaterial : public CEngineEntity, public CShared
	{
		public:
			CString										Name;
			CShader *									Shader = null;
			
			bool										AlphaBlending = false;

			CMap<CString, CConstantBinding<float>>		Floats;
			CMap<CString, CConstantBinding<CFloat3>>	Float3s;
			CMap<CString, CConstantBinding<CFloat4>>	Float4s;
			CMap<CString, CConstantBinding<CMatrix>>	Matrixes;
			CMap<CString, CTextureBinding>				Textures;
			CMap<CString, CSamplerBinding>				Samplers;

			CDirectConstantBuffer *						Buffer = null;

			UOS_RTTI
			CMaterial(CEngineLevel * l, CShader * sh, CString const & name = CGuid::Generate64(GetClassName()));
			CMaterial(CEngineLevel * l, CString const & name = CGuid::Generate64(GetClassName()));
			virtual ~CMaterial();

			void										Bind(CShader * s);
			void										Apply(CDirectDevice * d);
			
			void										Save(CXon * r, IMaterialStore * st);
			void										Load(CXon * r, IMaterialStore * st);

	};
}

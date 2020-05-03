#pragma once
#include "DirectSystem.h"
#include "Canvas.h"

namespace uos
{
	enum class ETextureFeature
	{
		Null, Load = 1, Canvas = (1<<1)
	};

	class UOS_ENGINE_LINKING CTexture : public CEngineEntity, public CShared
	{
		public:
			CString										Name;
			CDirectSystem *								GraphicEngine;

			DXGI_FORMAT									Format = DXGI_FORMAT_UNKNOWN;
			int											W = 0;
			int											H = 0;
			int											D = 0;
			int											MipLevels = 1; 

			ID3D11Texture2D *							DCTexture = null;
			IDXGISurface1 *								DCSurface = null;
			HDC											DC = null;

			ETextureFeature								Features = ETextureFeature::Null;

			CDirectDevice *										Owner = null;
			CMap<CDirectDevice *, ID3D11Resource *>				DxTextures;
			CMap<CDirectDevice *, ID3D11ShaderResourceView *>	DxTextureViews;

			ID3D11RenderTargetView *					DCRenderTargetView[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
			ID3D11DepthStencilView *					DCDepthStencilView;
			
			CCanvas *									Canvas = null;

			HANDLE										Shared = null;

			union 
			{
				D3D11_TEXTURE1D_DESC Desc1D;
				D3D11_TEXTURE2D_DESC Desc2D;
				D3D11_TEXTURE3D_DESC Desc3D;
			};

			D3D11_SHADER_RESOURCE_VIEW_DESC				Srvd;

			static const DXGI_FORMAT					DefaultFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
			static const int							Auto = -1;

			UOS_RTTI
			CTexture(CEngineLevel * l, CDirectSystem * ge, CString const & name = CGuid::Generate64(GetClassName()));
			virtual ~CTexture();

			bool										IsEmpty();
			void										Open(HANDLE shared);
			void										Create(int w, int h, int d, int mln, ETextureFeature f, DXGI_FORMAT format);
			void										Create(CDirectDevice * d);
			void										Resize(int w, int h, int d, bool copyPrev);
			CFloat3										GetSize();
			
			void										Load(void * data, int64_t size/*, int w = Auto, int h = Auto, int d = Auto*/);
			void										Load(CStream * stream);
			void										LoadAsync(CAsyncFileStream * stream, std::function<void()> ondone);
			void										Load(HMODULE module, int id);
			void										LoadPixels(void * data, int pitch);
			void										LoadArray(CStream * xp, CStream * xn, CStream * yp, CStream * yn, CStream * zp, CStream * zn);

			//CArray<char>								Save();
			void										Save(const CUol & o);
			void										Save(CStream * s);
			void										Save(CXon * p);
			void										Load(CXon * p);

			void										Fill(CFloat4 & color);


			void										Apply(CDirectDevice * gd, int i);
			void										Clear();

			void										Copy(CDirectDevice * srcd, ID3D11Resource * srct, CDirectDevice * dstd, ID3D11Resource * dstt);

			CCanvas *									BeginDraw();
			void										EndDraw();
	};
}
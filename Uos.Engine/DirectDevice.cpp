#include "StdAfx.h"
#include "DirectDevice.h"
#include "DirectSystem.h"

using namespace uos;

/*
CDirectDevice::CDirectDevice(CEngineLevel * l, CDirectEngine * ge, CDisplayDevice * dd) : CEngineEntity(l)
{
	///Engine			= ge;
	///Device			= dd;
	///DdKernel		= ddk;
	///auto g = l->Config->Root->One(L"GraphicEngine/GraphicDriver");
	///
	///SaveFrontBuffer		= false;
	///PresentBundle		= null;
	///PresentEnabled		= g->One(L"IsPresentEnabled")->AsBool();
	///SwapJumpEnabled		= g->One(L"IsSwapJumpEnabled")->AsBool();
	///VSync				= g->One(L"IsVSync")->AsBool();
	///
	///CScreenTarget * t = new CScreenTarget(Level, this, null);
	///
	///InitFullscreenPresentParameters(t, dd);
	///
	///HRESULT hr = Engine->D3D->CreateDevice(dd->Dx9Index, D3DDEVTYPE_HAL, GetDesktopWindow(), GetDeviceFlags(dd), &t->PresentParameters, &D3DDevice);
	///if(hr == S_OK)
	///{
	///	InitCommon(t);
	///	Targets.push_back(t);
	///	Level->Log->ReportMessage(this, L"Created as %s", dd->Name.c_str());
	///}
	///else
	///	throw CAttentionException(HERE, L"Could not create 3d device. 3d device is not available.");
}*/

CDirectDevice::CDirectDevice(CEngineLevel * l, CDirectSystem * ge, CDisplayDevice * dd) : CEngineEntity(l)
{
	Engine = ge;
	Display = dd;
	SaveFrontBuffer = false;
	PresentEnabled = true;
	SwapJumpEnabled = false;

	auto g = l->Config->Root->One(L"GraphicEngine/GraphicDriver");
	VSync = g->One(L"IsVSync")->AsBool();

	///auto t = new CDirect3D9ScreenTarget(Level, this, s);
	///InitWindowPresentParameters(t, s, dd->Format);


	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
	Level->Log->ReportWarning(this, L"Debug mode enabled");
#endif

	D3D_FEATURE_LEVEL feature_levels[] ={
											D3D_FEATURE_LEVEL_11_1,
											D3D_FEATURE_LEVEL_11_0,
											D3D_FEATURE_LEVEL_10_1,
											D3D_FEATURE_LEVEL_10_0
											//D3D_FEATURE_LEVEL_9_3
											//D3D_FEATURE_LEVEL_9_2,
											//D3D_FEATURE_LEVEL_9_1
										};

	UINT num_feature_levels = sizeof(feature_levels) / sizeof(feature_levels[0]);

	HRESULT hr = D3D11CreateDevice(dd->Adapter->DxAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, feature_levels, num_feature_levels, D3D11_SDK_VERSION, &DxDevice, &FeatureLevel, &DxContext);

	if(hr == E_INVALIDARG)
	{
		// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 
		// so we need to retry without it
		hr = D3D11CreateDevice(dd->Adapter->DxAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, &feature_levels[1], num_feature_levels - 1, D3D11_SDK_VERSION, &DxDevice, &FeatureLevel, &DxContext);
	}

	if(!DxDevice)
	{
		throw CAttentionException(HERE, L"Required DirectX version is not supported");
	}


#ifdef _DEBUG
	DxDevice->SetExceptionMode(D3D11_RAISE_FLAG_DRIVER_INTERNAL_ERROR);
#endif

	for(int i=1; i<=16; i*=2)
	{
		UINT msq;
		DxDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, i, &msq);

		if(msq > 0)
		{
			MSAA[i] = msq;
		}
	}


	//HRESULT hr = Engine->D3D->CreateDeviceEx(dd->Dx9Index, D3DDEVTYPE_HAL, Level->Core->Mmc->GetHwnd(), GetDeviceFlags(dd), &t->PresentParameters, null, &D3DDevice);	
}

CDirectDevice::~CDirectDevice()
{
	for(auto i : Targets)
	{
		delete i;
	}

#ifdef _DEBUG
	//ID3D11Debug * m_d3dDebug;
	//DxDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&m_d3dDebug));
	//
	//m_d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	//m_d3dDebug->Release();
#endif

	DxDevice->Release();
	DxContext->Release();

	//Level->Core->ExitQueried -= ThisHandler(OnLevel1ExitRequested);
	//Level->Core->Suspended -= ThisHandler(OnLevel1Suspending);
}

CScreenRenderTarget * CDirectDevice::AddTarget(CWindowScreen * s)
{
	auto t = new CScreenRenderTarget(Level, this, s);
	Targets.push_back(t);
	Level->Log->ReportMessage(this, L"Target Created: Window=%s  Device=%s", s->GetTitle(), Display->Name);
	return t;
}

void CDirectDevice::RemoveTarget(CScreenRenderTarget * t)
{
	Targets.Remove(t);
	delete t;
}

//void CDirectDevice::InitFullscreenPresentParameters(CDirectScreenTarget * t, CDisplayDevice * dd)
//{
//	t->PresentParameters.Windowed              		= false;
//	t->PresentParameters.EnableAutoDepthStencil		= false;
//	t->PresentParameters.AutoDepthStencilFormat		= D3DFMT_UNKNOWN;
//	t->PresentParameters.SwapEffect					= D3DSWAPEFFECT_FLIPEX;
//	t->PresentParameters.FullScreen_RefreshRateInHz	= dd->RefreshRate;
//	t->PresentParameters.BackBufferFormat			= dd->Format;
//	t->PresentParameters.BackBufferWidth			= dd->NRect.Width;
//	t->PresentParameters.BackBufferHeight		 	= dd->NRect.Height;
//	t->PresentParameters.BackBufferCount			= 2;
//	t->PresentParameters.PresentationInterval		= VSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
//	t->PresentParameters.Flags						= 0;
//}
//
//void CDirectDevice::InitWindowPresentParameters(CDirectScreenTarget * t, CWindowScreen * w, D3DFORMAT f)
//{
//	t->PresentParameters.Windowed				= true;
//	t->PresentParameters.hDeviceWindow			= w->GetHwnd();  
//	t->PresentParameters.EnableAutoDepthStencil	= false;
//	t->PresentParameters.AutoDepthStencilFormat	= D3DFMT_UNKNOWN;
//	t->PresentParameters.SwapEffect				= D3DSWAPEFFECT_FLIPEX;
//	t->PresentParameters.BackBufferWidth		= w->GetClientRect().Width;
//	t->PresentParameters.BackBufferHeight		= w->GetClientRect().Height;
//	t->PresentParameters.BackBufferFormat		= f;
//	t->PresentParameters.PresentationInterval	= VSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;;
//	t->PresentParameters.BackBufferCount		= 1;
//	t->PresentParameters.Flags					= 0;
//}

void CDirectDevice::InitCommon(CScreenRenderTarget * t)
{
	///Verify(D3DDevice->GetSwapChain(0, &t->SwapChain));
	///Verify(t->SwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &t->BackBufferA));
	///if(t->PresentParameters.BackBufferCount >= 2)
	///{
	///	Verify(D3DDevice->GetBackBuffer(0, 1, D3DBACKBUFFER_TYPE_MONO, &t->BackBufferB));
	///}
	///			
	///Verify(D3DDevice->CreateDepthStencilSurface(	t->PresentParameters.BackBufferWidth, 
	///													t->PresentParameters.BackBufferHeight, 
	///													GetZBufferFormat(Display),
	///													D3DMULTISAMPLE_NONE,
	///													0,
	///													true,
	///													&ZBuffer,
	///													null));	
	///
	///Verify(D3DDevice->SetDepthStencilSurface(ZBuffer));
	///													
	///InitCache();

	///Level->Core->ExitQueried += ThisHandler(OnLevel1ExitRequested);
	///Level->Core->Suspended += ThisHandler(OnLevel1Suspending);
}
/*

D3DFORMAT CDirectDevice::GetZBufferFormat(CDisplayDevice * dd)
{
/ *
	if(SUCCEEDED(Engine->D3D->CheckDeviceFormat(dd->DxIndex, D3DDEVTYPE_HAL, dd->Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32)))
	{
		return D3DFMT_D32;
	}
* /
	if(SUCCEEDED(Engine->D3D->CheckDeviceFormat(dd->Dx9Index, D3DDEVTYPE_HAL, dd->Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8)))
	{
		return D3DFMT_D24S8;
	}
	return D3DFMT_D16;
}
*/

bool CDirectDevice::TestVideoMemorySpeed()
{
	///IDirect3DQuery9* pQuery = null;  
	///
	///if(	D3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &pQuery) == D3D_OK &&
	///	Display->Capabilities.DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES /*&&
	///	Engine->D3D->CheckDeviceFormatConversion(dd->DxIndex, D3DDEVTYPE_HAL, Targets[0].PresentParameters.BackBufferFormat, Targets[0].PresentParameters.BackBufferFormat) == D3D_OK*/)
	///{
	///	CGdiRect r(0, 0, 2048, 2048);
	///
	///	float framesize = float(r.GetWidth()*r.GetHeight())*4;
	///
	///	LPDIRECT3DSURFACE9 sa;
	///	LPDIRECT3DSURFACE9 sb;
	///	Verify(D3DDevice->CreateOffscreenPlainSurface(r.GetWidth(), r.GetHeight(), Display->Format, D3DPOOL_DEFAULT, &sa, null));
	///	Verify(D3DDevice->CreateOffscreenPlainSurface(r.GetWidth(), r.GetHeight(), Display->Format, D3DPOOL_DEFAULT, &sb, null));
	///
	///	Verify(pQuery->Issue(D3DISSUE_END));
	///	while(pQuery->GetData(NULL, 0, D3DGETDATA_FLUSH) == S_FALSE);
	///
	///	float start = Level->Core->Timer.GetTime();
	///
	///	int n = 4096;
	///	for(int i=0; i<n; i++)
	///	{
	///		Verify(D3DDevice->StretchRect(sa, null, sb, null, D3DTEXF_NONE));
	///	}
	///
	///	Verify(pQuery->Issue(D3DISSUE_END));
	///	while(pQuery->GetData(NULL, 0, D3DGETDATA_FLUSH) == S_FALSE);
	///	
	///	pQuery->Release();
	///
	///	float speed = (framesize*n)/(Level->Core->Timer.GetTime() - start);
	///
	///	sa->Release();
	///	sb->Release();
	///	
	///	float fps = speed/framesize;
	///	
	///	Level->Log->ReportMessage(this, L"FillFPS: %.1f    FillRate: %.1f MB/s", fps, speed/_1M);
	///	return fps > 100.f;
	///}
	return false;
}

void CDirectDevice::OnLevel1ExitRequested()
{
	SwitchToGDI();
}

void CDirectDevice::OnLevel1Suspending()
{
	SwitchToGDI();
}

int CDirectDevice::GetVideoMemoryFreeAmount()
{
	return -1;
}

void CDirectDevice::SwitchToGDI()
{
	throw CException(HERE, L"Not implemented");
}

CDisplayDevice * CDirectDevice::GetDisplayDevice()
{
	return Display;
}

int CDirectDevice::GetPixelSize(DXGI_FORMAT f)
{
	switch(f)
	{
		case DXGI_FORMAT_R8G8B8A8_TYPELESS  :
		case DXGI_FORMAT_R8G8B8A8_UNORM     :
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT      :
		case DXGI_FORMAT_R8G8B8A8_SNORM     :
		case DXGI_FORMAT_R8G8B8A8_SINT      :

		case DXGI_FORMAT_B8G8R8A8_UNORM      :
		case DXGI_FORMAT_B8G8R8X8_UNORM      :
		case DXGI_FORMAT_B8G8R8A8_TYPELESS   :
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB :
		case DXGI_FORMAT_B8G8R8X8_TYPELESS   :
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB :
			return 4;
	}

	throw CException(HERE, L"Not supported");
}

CArray<char> CDirectDevice::SaveImage(ID3D11Resource * r)
{
	auto stg = CaptureTexture(r);

	D3D11_TEXTURE2D_DESC d;
	stg->GetDesc(&d);
	

	D3D11_MAPPED_SUBRESOURCE m;
	Verify(DxContext->Map(stg, 0, D3D11_MAP_READ, 0, &m));

	auto o = CArray<char>(d.Width * d.Height * 4);

	auto pxsize = GetPixelSize(d.Format);

	for(auto i=0u; i<d.Height; i++)
	{
		//CopyMemory(p + i * Width * pxsize, (char *)m.pData + i * m.RowPitch, Width * pxsize); // flip horizontally
		CopyMemory(o.data() + i * d.Width * pxsize, (char *)m.pData + (d.Height - i - 1) * m.RowPitch, d.Width * pxsize); // flip horizontally
	}

	DxContext->Unmap(stg, 0);

	ILuint handle;
	ilGenImages(1, &handle);
	ilBindImage(handle);
	ilTexImage(d.Width, d.Height, 0, pxsize, IL_BGRA, IL_UNSIGNED_BYTE, o.data());
	int n = ilSaveL(IL_JPG, null, 0);

	o.resize(n);
	ilSaveL(IL_JPG, o.data(), n);
	ilDeleteImages(1, &handle);

	stg->Release();

	return o;
}

void CDirectDevice::TakeScreenShot(CRenderTarget * st, CString & url)
{
	auto dst = st->As<CScreenRenderTarget>();

	auto t = CaptureTexture(dst->BackBufferA);
	
	if(t)
	{
		auto s = Level->Nexus->Storage->OpenWriteStream(url);
	
		auto img = SaveImage(t);

		s->Write(img.data(), img.size());
	
		Level->Nexus->Storage->Close(s);
		
		t->Release();
	
		Level->Log->ReportMessage(this, L"Screenshot taken: %s", url);
	}
}

void CDirectDevice::SaveDepthStencil(const CString & path)
{
	///int n=0;
	///for(auto i : Targets)
	///{
	///	CString p = CPath::GetSafe(CPath::ReplaceFileName(path, CPath::GetFileName(path) + CConverter::ToString(n++)));
	///	LPDIRECT3DSURFACE9 s;
	///	D3DDevice->GetDepthStencilSurface(&s);
	///	
	///	IDirect3DSurface9* o;
	///	HRESULT hr = D3DDevice->CreateOffscreenPlainSurface(i->PresentParameters.BackBufferWidth, i->PresentParameters.BackBufferHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &o, NULL );
	///	Verify(D3DDevice->GetRenderTargetData(s, o));
	///
	///	Verify(D3DXSaveSurfaceToFile(p.c_str(), D3DXIFF_PNG, o, null, null));
	///	
	///	s->Release();
	///}		
}

ID3D11Texture2D * CDirectDevice::CaptureTexture(ID3D11Resource* pSource)
{
	D3D11_RESOURCE_DIMENSION rtype = D3D11_RESOURCE_DIMENSION_UNKNOWN;
	pSource->GetType(&rtype);

	if(rtype != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
		throw CException(HERE, L"resType != D3D11_RESOURCE_DIMENSION_TEXTURE2D");

	ID3D11Texture2D * texture = null;
	ID3D11Texture2D * staging = null;

	HRESULT hr = pSource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture));

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	if(desc.SampleDesc.Count > 1)
	{
		// MSAA content must be resolved before being copied to a staging texture
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		ID3D11Texture2D * temp;
		Verify(DxDevice->CreateTexture2D(&desc, 0, &temp));

		DXGI_FORMAT fmt = desc.Format;

		UINT support = 0;
		Verify(DxDevice->CheckFormatSupport(fmt, &support));

		if(!(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE))
			throw CException(HERE, L"Failed D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE");

		for(UINT item = 0; item < desc.ArraySize; ++item)
		{
			for(UINT level = 0; level < desc.MipLevels; ++level)
			{
				UINT index = D3D11CalcSubresource(level, item, desc.MipLevels);
				DxContext->ResolveSubresource(temp, index, pSource, index, fmt);
			}
		}

		desc.BindFlags = 0;
		desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;

		Verify(DxDevice->CreateTexture2D(&desc, 0, &staging));

		DxContext->CopyResource(staging, temp);

		temp->Release();
	}
	else if((desc.Usage == D3D11_USAGE_STAGING) && (desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ))
	{
		// Handle case where the source is already a staging texture we can use directly
		staging = texture;
		staging->AddRef();
	}
	else
	{
		// Otherwise, create a staging texture from the non-MSAA source
		desc.BindFlags = 0;
		desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;

		Verify(DxDevice->CreateTexture2D(&desc, 0, &staging));

		DxContext->CopyResource(staging, pSource);
	}

	if(texture)
	{
		texture->Release();
	}

	return staging;
}
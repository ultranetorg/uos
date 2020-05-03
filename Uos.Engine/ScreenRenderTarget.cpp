#include "stdafx.h"
#include "ScreenRenderTarget.h"
#include "DirectDevice.h"

using namespace uos;

CScreenRenderTarget::CScreenRenderTarget(CEngineLevel * l, CDirectDevice * d, CWindowScreen * s)
{
	Level = l;
	Device = d;
	Screen = s;

	IDXGIFactory1* factory1 = nullptr;
	IDXGIDevice* dev = nullptr;
	
	Verify(d->Display->Adapter->DxAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&factory1)));
	
	IDXGIFactory2* factory2 = nullptr;
	factory1->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&factory2));

	auto msc = 1;
	auto msq = 0;
	auto msaa = Level->Config->Root->Get<CString>(L"GraphicEngine/MSAA");

	if(msaa == L"Auto")
	{
		if(d->MSAA.Contains(4))
			msc = 4;
		else if(d->MSAA.Contains(2))
			msc = 2;

		if(msc > 1)
		{
			msq = d->MSAA[msc]-1;
		}
	}
	else if(msaa == L"Off")
	{
	}
	else
	{
		if(d->MSAA.Contains(CInt32::Parse(msaa)))
		{
			msc = CInt32::Parse(msaa);
			msq = d->MSAA[msc]-1;
		}
	}

	//Size.W = s->Rect.Width * 96 / d->Display->Dpi.x;
	//Size.H = s->Rect.Height * 96 / d->Display->Dpi.y;

	Size.H = float(s->Rect.Height);
	Size.W = float(s->Rect.Width);

	//Size.W = d->Display->NRect.Width;
	//Size.H = d->Display->NRect.Height;

	if(factory2)
	{
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width				= UINT(Size.W);
		sd.Height				= UINT(Size.H);
		sd.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.SampleDesc.Count		= msc;
		sd.SampleDesc.Quality	= msq;
		sd.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount			= 1;

		IDXGISwapChain1* swapchain1 = nullptr;

		if(SUCCEEDED(factory2->CreateSwapChainForHwnd(d->DxDevice, s->Hwnd, &sd, nullptr, nullptr, &swapchain1)))
		{
			swapchain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&SwapChain));
			swapchain1->Release();
		}

		factory2->Release();
	}
	else
	{
		// DirectX 11.0 systems
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferCount = 1;
		sd.BufferDesc.Width	= UINT(Size.W);
		sd.BufferDesc.Height= UINT(Size.H);
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = s->Hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		Verify(factory1->CreateSwapChain(d->DxDevice, &sd, &SwapChain));
	}

	Verify(factory1->MakeWindowAssociation(s->Hwnd, DXGI_MWA_NO_ALT_ENTER));

	Verify(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBufferA)));

	Verify(d->DxDevice->CreateRenderTargetView(BackBufferA, nullptr, &RenderTargetView));


	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width				= UINT(Size.W);
	descDepth.Height			= UINT(Size.H);
	descDepth.MipLevels			= 1;
	descDepth.ArraySize			= 1;
	descDepth.Format			= DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	//descDepth.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count	= msc;
	descDepth.SampleDesc.Quality= msq;
	descDepth.Usage				= D3D11_USAGE_DEFAULT;
	descDepth.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags	= 0;
	descDepth.MiscFlags			= 0;
	Verify(d->DxDevice->CreateTexture2D(&descDepth, NULL, &DepthStencil));
	
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;

	Verify(Device->DxDevice->CreateDepthStencilView(DepthStencil, &descDSV, &DepthStencilView));

	factory1->Release();
}

CScreenRenderTarget::~CScreenRenderTarget()
{
	DepthStencilView->Release();
	DepthStencil->Release();
	RenderTargetView->Release();
	BackBufferA->Release();
	SwapChain->Release();
}

void CScreenRenderTarget::Apply()
{
	Device->DxContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
}

void CScreenRenderTarget::Present()
{
	auto r = SwapChain->Present(Device->VSync ? 1 : 0, 0);
	if(!(r == S_OK || r == DXGI_STATUS_OCCLUDED))
	{
		Verify(r);
	}
}

void CScreenRenderTarget::TakeScreenshot(CString & o)
{
	Device->TakeScreenShot(this, o);
}

void CScreenRenderTarget::SetViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	///D3D11_VIEWPORT v = {vp->X, Screen->Rect.Height - vp->Y - vp->H, vp->W, vp->H, vp->MinZ, vp->MaxZ};

	D3D11_VIEWPORT v = {topLeftX, topLeftY, width, height, minDepth, maxDepth};

	Device->DxContext->RSSetViewports(1, &v);
}

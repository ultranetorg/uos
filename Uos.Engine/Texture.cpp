#include "StdAfx.h"
#include "Texture.h"

using namespace uos;

CTexture::CTexture(CEngineLevel * l, CDirectSystem * ge, CString const & name) : CEngineEntity(l)
{
	GraphicEngine = ge;
	Name = name;
}

CTexture::~CTexture()
{
	Clear();
}

void CTexture::Clear()
{
	for(auto & i : DxTextures)
		if(i.second)
		{
			i.second->Release();
		}

	for(auto & i : DxTextureViews)
		if(i.second)
		{
			i.second->Release();
		}

	DxTextures.clear();
	DxTextureViews.clear();

	H = 0;
	W = 0;
	D = 0;
}

CFloat3 CTexture::GetSize()
{
	return CFloat3(float(W), float(H), float(D));
}

bool CTexture::IsEmpty()
{
	return W == 0;
}

void CTexture::Create(int w, int h, int d, int mips, ETextureFeature f, DXGI_FORMAT format)
{
	if(w < 1 || h < 1)
	{
		throw CException(HERE, L"width < 1 or height < 1");
	}

	Clear();

	W = w;
	H = h;
	D = d;
	MipLevels = mips;
	Features = f;
	Format = format;

	Srvd.Format = format;

	Owner = GraphicEngine->Devices.front();

	auto usage = D3D11_USAGE_DEFAULT;
	UINT bind = D3D11_BIND_SHADER_RESOURCE;
	UINT cpu = 0;
	UINT misc =  D3D11_RESOURCE_MISC_SHARED;

	if(int(f) & int(ETextureFeature::Load))
	{
		//usage = D3D11_USAGE_DYNAMIC;
		//cpu |= D3D11_CPU_ACCESS_WRITE;
	}

	if(int(f) & int(ETextureFeature::Canvas))
	{
		bind |= D3D11_BIND_RENDER_TARGET;
		misc |= D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
	}
		
	if(d == 0)
	{
		Desc2D.Width = w;
		Desc2D.Height = h;
		Desc2D.MipLevels = mips;
		Desc2D.ArraySize = 1;
		Desc2D.Format = Format;
		Desc2D.CPUAccessFlags = 0;
		Desc2D.SampleDesc.Count = 1;
		Desc2D.SampleDesc.Quality = 0;
		Desc2D.Usage = usage;
		Desc2D.BindFlags = bind;
		Desc2D.CPUAccessFlags = cpu;
		Desc2D.MiscFlags = misc;

		Srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		Srvd.Texture2D.MipLevels = mips;
		Srvd.Texture2D.MostDetailedMip = 0;
	}
	else if(d > 0)
	{
		Desc3D.Width = w;
		Desc3D.Height = h;
		Desc3D.Depth = d;
		Desc3D.MipLevels = mips;
		//Desc3D.ArraySize = 1;
		Desc3D.Format = Format;
		Desc3D.CPUAccessFlags = 0;
		//Desc3D.SampleDesc.Count = 1;
		//Desc3D.SampleDesc.Quality = 0;
		Desc3D.Usage = usage;
		Desc3D.BindFlags = bind;
		Desc3D.CPUAccessFlags = cpu;
		Desc3D.MiscFlags = misc;
		
		Srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		Srvd.Texture3D.MipLevels = mips;
		Srvd.Texture3D.MostDetailedMip = 0;
	}

	Create(Owner);
	   
#ifdef _DEBUG
	//DxTextureViews[GraphicEngine->Devices.front()]->SetPrivateData(WKPDID_D3DDebugObjectNameW, Name.size() - 1, Name.data());
#endif
}

void CTexture::Create(CDirectDevice * d)
{
	if(D == 0)
	{
		ID3D11Texture2D * t;
		Verify(d->DxDevice->CreateTexture2D(&Desc2D, null, &t));
		DxTextures[d] = t;
	} 
	else
	{
		ID3D11Texture3D * t;
		Verify(Owner->DxDevice->CreateTexture3D(&Desc3D, null, &t));
		DxTextures[d] = t;
	}

	Verify(d->DxDevice->CreateShaderResourceView(DxTextures(d), &Srvd, &DxTextureViews[d]));

	IDXGIResource * r;
	Verify(DxTextures(d)->QueryInterface(__uuidof(IDXGIResource), (void**)&r));
	Verify(r->GetSharedHandle(&Shared));
	r->Release();
}

void CTexture::Apply(CDirectDevice * d, int slot)
{
	if(!DxTextures.empty())
	{
		if(!DxTextures.Contains(d))
		{
			if(Shared)
			{
				Verify(d->DxDevice->OpenSharedResource(Shared, D == 0 ? __uuidof(ID3D11Texture2D) : __uuidof(ID3D11Texture3D), (void**)(&DxTextures[d])));
				Verify(d->DxDevice->CreateShaderResourceView(DxTextures(d), &Srvd, &DxTextureViews[d]));
			}
			else
			{
				Create(d);
				Copy(DxTextures.begin()->first, DxTextures.begin()->second, d, DxTextures(d));
				
/*
				#pragma message ("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Debug only code >>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
				#ifdef _DEBUG
				{
					auto & jpg = d->SaveImage(DxTextures(d));
					auto s = Level->Nexus->Storage->OpenWriteStream(Level->Nexus->Storage->MapTmpPath(CFile::GetClassName(), CString::Format(L"Texture-%p-0.jpg", DxTextures(d))));
					s->Write(jpg.data(), jpg.size());
					Level->Nexus->Storage->Close(s);
				}
				#endif*/
			}
		}
	
		d->DxContext->PSSetShaderResources(slot, 1, &DxTextureViews(d));
	}
	else
	{
		ID3D11ShaderResourceView * p[1] = {null};
		d->DxContext->PSSetShaderResources(slot, 1, p);
	}
}

void CTexture::Open(HANDLE shared)
{
	Clear();

	Shared = shared;

	ID3D11Texture2D * t = nullptr;
	Verify(GraphicEngine->Devices.front()->DxDevice->OpenSharedResource(shared, __uuidof(ID3D11Texture2D), (void**)(&t)));

	t->GetDesc(&Desc2D);

	ID3D11ShaderResourceView* srv = nullptr;

	Srvd = {};
	Srvd.Format						= Format;
	Srvd.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	Srvd.Texture2D.MostDetailedMip	= 0;
	Srvd.Texture2D.MipLevels		= 1;

	Verify(GraphicEngine->Devices.front()->DxDevice->CreateShaderResourceView(t, &Srvd, &srv));

	DxTextures[GraphicEngine->Devices.front()] = t;
	DxTextureViews[GraphicEngine->Devices.front()] = srv;
}

void CTexture::Resize(int w, int h, int d, bool preserve)
{
	if(W == w && H == h && D == d)
	{
		return;
	}

	if(IsEmpty())
	{
		throw CException(HERE, L"Texture is not created");
	}

	ILuint handle;
	ID3D11Texture2D * t = null;

	if(preserve)
	{
		auto desc = Desc2D;
		desc.BindFlags = 0;
		desc.MiscFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;
	
		Verify(DxTextures.begin()->first->DxDevice->CreateTexture2D(&desc, 0, &t));
	
		DxTextures.begin()->first->DxContext->CopyResource(t, DxTextures.begin()->second);
	
		D3D11_MAPPED_SUBRESOURCE m;
		Verify(DxTextures.begin()->first->DxContext->Map(t, 0, D3D11_MAP_READ, 0, &m));
	
		auto o = CArray<char>(W * H * 4);
	
		auto pxsize = DxTextures.begin()->first->GetPixelSize(Format);
	
		for(int i=0; i<H; i++)
		{
			CopyMemory(o.data() + i * W * pxsize, (char *)m.pData + i * m.RowPitch, W * pxsize); // no vertical flip here
		}
	
		DxTextures.begin()->first->DxContext->Unmap(t, 0);
	
		ilGenImages(1, &handle);
		ilBindImage(handle);
		ilTexImage(W, H, D, pxsize, IL_BGRA, IL_UNSIGNED_BYTE, o.data());
	
		iluImageParameter(ILU_FILTER, ILU_BILINEAR);
		iluScale(w, h, d);
	}

	Create(w, h, d, MipLevels, Features, Format);

	if(preserve)
	{
		LoadPixels(ilGetData(), w * ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL));
		ilDeleteImage(handle);
		t->Release();
	}
}

void CTexture::Copy(CDirectDevice * srcd, ID3D11Resource * srct, CDirectDevice * dstd, ID3D11Resource * dstt)
{
	auto t = srcd->CaptureTexture(srct);

	D3D11_MAPPED_SUBRESOURCE m;

	int n = 0;
	if(H == 0)	n = Desc1D.ArraySize; else 
	if(D == 0)	n = Desc2D.ArraySize;

	for(int i=0; i<n; i++)
	{
		Verify(srcd->DxContext->Map(t, i, D3D11_MAP_READ, 0, &m));
		dstd->DxContext->UpdateSubresource(dstt, i, NULL, m.pData, m.RowPitch, m.DepthPitch);
		srcd->DxContext->Unmap(t, i);
	}

	t->Release();
}

void CTexture::Load(void * data, int64_t size/*, int w, int h, int d*/)
{
	assert(size <= UINT_MAX); //ILuint
	
	ILuint handle;
	ilGenImages(1, &handle);
	ilBindImage(handle);
	ilLoadL(IL_TYPE_UNKNOWN, data, (ILuint)size);
	ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
	iluFlipImage();

	auto w = ilGetInteger(IL_IMAGE_WIDTH);
	auto h = ilGetInteger(IL_IMAGE_HEIGHT);
	auto d = ilGetInteger(IL_IMAGE_DEPTH);
	if(d == 1)
		d = 0;

	if(IsEmpty())
		Create(w, h, d, 1, ETextureFeature::Load, DefaultFormat);
	else
		Resize(w, h, d, false);

	LoadPixels(ilGetData(), w * ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL));

	ilDeleteImage(handle);
}

void CTexture::Load(CStream * stream/*, int w, int h, int d*/)
{
	auto b = stream->Read();
	Load(b.GetData(), b.GetSize()/*, w, h, d*/);
}

void CTexture::LoadAsync(CAsyncFileStream * stream, std::function<void()> ondone)
{
	stream->ReadAsync(	[this, stream, ondone]()
						{
							Load(stream->Buffer.GetData(), stream->Buffer.GetSize());
							ondone();
						});
}

void CTexture::LoadPixels(void * data, int pitch)
{
	auto d = DxTextureViews.begin()->first;
	auto r = DxTextures.begin()->second;

	d->DxContext->UpdateSubresource(r, 0, NULL, data, pitch, 0);
/*
	D3D11_MAPPED_SUBRESOURCE m;
	Verify(d->DxContext->Map(r, 0, D3D11_MAP_WRITE_DISCARD, 0, &m));

	auto dst = (char *)m.pData;
	auto src = (char *)data;

	auto n = W * DxTextures.begin()->first->GetPixelSize(Format);

	for(int i=0; i < (D == 0 ? 1 : D); i++)
	{
		auto src = (char *)data		+ i * H * pitch;
		auto dst = (char *)m.pData	+ i * m.DepthPitch;
		
		for(int j=0; j<H; j++)
		{
			CopyMemory(dst, src, n);
			src += pitch;
			dst += m.RowPitch;
		}
	}

	d->DxContext->Unmap(r, 0);*/
}

void CTexture::Load(HMODULE module, int id)
{
	Clear();
	HRSRC r = FindResource(module, MAKEINTRESOURCE(id), L"PNG"); 
	if(r != null)
	{
		HGLOBAL rl = LoadResource(module, r);
		if(rl != null)
		{
			Load(LockResource(rl), SizeofResource(module, r));
		}
	}
}

void CTexture::Fill(CFloat4 & color)
{
	auto d = DxTextures.begin()->first;
	auto r = DxTextures.begin()->second;

	D3D11_MAPPED_SUBRESOURCE m;
	Verify(d->DxContext->Map(r, 0, D3D11_MAP_WRITE_DISCARD, 0, &m));

	auto dst = (unsigned char *)m.pData;

	auto n = W * DxTextures.begin()->first->GetPixelSize(Format);

	for(int i=0; i < (D == 0 ? 1 : D); i++)
	{
		auto dst = (unsigned char *)m.pData	+ i * m.DepthPitch;

		for(int j=0; j<H; j++)
		{
			auto p = dst;

			for(int i=0; i<W; i++)
			{
				*(p + 0) = unsigned char(255 * color.z);
				*(p + 1) = unsigned char(255 * color.y);
				*(p + 2) = unsigned char(255 * color.x);
				*(p + 3) = unsigned char(255 * color.w);
				p+=4;
			}
			dst += m.RowPitch;
		}
	}

	d->DxContext->Unmap(r, 0);
}	

void CTexture::Save(const CUol & o)
{
	throw CException(HERE, L"Not implemented");	
	///ilSaveL()
}

void CTexture::Save(CStream * s)
{
	auto & d = Owner->SaveImage(DxTextures(Owner));
	s->Write(d.data(), d.size());
}

void CTexture::Save(CXon * xon)
{
	xon->Set(Name);
	xon->Add(L"Format")->Set(L"PNG");

	if(!IsEmpty())
	{
		auto & d = Owner->SaveImage(DxTextures(Owner));
		xon->Add(L"Data")->Set(CSerializableBuffer(d.data(), d.size()));
	}
	else
		xon->Add(L"Data")->Set(CSerializableBuffer());
}

void CTexture::Load(CXon * p)
{
	Name = p->AsString();
	auto d = p->One(L"Data")->Get<CSerializableBuffer>();

	if(d.GetSize() > 0)
	{
		Load(d.GetData(), d.GetSize());
	}
}

void CTexture::LoadArray(CStream * xp, CStream * xn, CStream * yp, CStream * yn, CStream * zp, CStream * zn)
{
	ILuint handle;
	ilGenImages(1, &handle);
	ilBindImage(handle);

	auto b = xp->Read();

	assert(b.GetSize() <= UINT_MAX);

	ilLoadL(IL_TYPE_UNKNOWN, b.GetData(), (ILuint)b.GetSize());
	

	//auto f = ilGetInteger(IL_IMAGE_FORMAT);

	auto p = ilGetData();

	W = ilGetInteger(IL_IMAGE_WIDTH);
	H = ilGetInteger(IL_IMAGE_HEIGHT);
	D = 0;
	auto bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

	Format = DefaultFormat;

	Desc2D.Width = W;
	Desc2D.Height = H;
	Desc2D.MipLevels = 1;
	Desc2D.ArraySize = 6;
	Desc2D.Format = DefaultFormat;
	Desc2D.CPUAccessFlags = 0;
	Desc2D.SampleDesc.Count = 1;
	Desc2D.SampleDesc.Quality = 0;
	Desc2D.Usage = D3D11_USAGE_DEFAULT;
	Desc2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc2D.CPUAccessFlags = 0;
	Desc2D.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	
	D3D11_SUBRESOURCE_DATA srd[6];
	CArray<char> buffer[6];

	auto load = [&](CStream * s, int i)
				{
					auto b = s->Read();	
					assert(b.GetSize() <= UINT_MAX);
					ilLoadL(IL_TYPE_UNKNOWN, b.GetData(), (ILuint)b.GetSize());	
					ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);

					buffer[i].resize(W * H * bpp);

					auto src = (char *)ilGetData();
					auto dst = buffer[i].data();
					auto pitch = W * bpp;

					//Save(&CFileStream(L"a:\\" + CConverter::ToString((size_t)i)+L".png", EFileMode::New));

					for(int i=0; i<H; i++)
					{
						CopyMemory(dst + i * pitch, src + (H - i - 1) * pitch, pitch);
					}

					//buffer[i].reSet(ilGetData(), ilGetInteger(IL_IMAGE_SIZE_OF_DATA));

					srd[i].pSysMem = buffer[i].data();
					srd[i].SysMemPitch = pitch;
					srd[i].SysMemSlicePitch = 0;
				};

	load(xp, 0);
	load(xn, 1);
	load(yp, 2);
	load(yn, 3);
	load(zp, 4);
	load(zn, 5);


	ID3D11Texture2D * t;
	Verify(GraphicEngine->Devices.front()->DxDevice->CreateTexture2D(&Desc2D, srd, &t));
	DxTextures[GraphicEngine->Devices.front()] = t;


	Srvd.Format = Desc2D.Format;
	Srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	Srvd.TextureCube.MipLevels = 1;
	Srvd.TextureCube.MostDetailedMip = 0;

	Verify(GraphicEngine->Devices.front()->DxDevice->CreateShaderResourceView(t, &Srvd, &DxTextureViews[GraphicEngine->Devices.front()]));

	ilDeleteImages(1, &handle);
}

CCanvas * CTexture::BeginDraw()
{
	auto t = DxTextures.begin();
	Canvas = new CCanvas(Level, GraphicEngine, t->first, t->second);
	return Canvas;
	//CanvasRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(0, -float(Height)) * D2D1::Matrix3x2F::Scale(1, -1));
}

void CTexture::EndDraw()
{
	delete Canvas;
}


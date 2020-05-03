#include "StdAfx.h"
#include "TextureFactory.h"

using namespace uos;

CTextureFactory::CTextureFactory(CEngineLevel * l, CDirectSystem * ge) : CEngineEntity(l)
{
	GraphicEngine = ge;

	Formats.push_back({L"Windows Bitmap",					L"bmp"					});
	Formats.push_back({L"Dr. Halo",							L"cut"					});
	Formats.push_back({L"Multi-PCX",						L"dcx"					});
	Formats.push_back({L"Dicom",							L"dicom dcm"			});
	Formats.push_back({L"DirectDraw Surface",				L"dds"					});
	Formats.push_back({L"OpenEXR",							L"exr"					});
	Formats.push_back({L"Flexible Image Transport System",	L"fits fit"				});
	Formats.push_back({L"Heavy Metal: FAKK 2",				L"ftx"					});
	Formats.push_back({L"Radiance High Dynamic",			L"hdr"					});
	Formats.push_back({L"Macintosh icon",					L"icns"					});
	Formats.push_back({L"Windows icon/cursor",				L"ico cur"				});
	Formats.push_back({L"Interchange File Format",			L"iff"					});
	Formats.push_back({L"Infinity Ward Image",				L"iwi"					});
	Formats.push_back({L"Graphics Interchange Format",		L"gif"					});
	Formats.push_back({L"Jpeg",								L"jpg jpe jpeg"			});
	Formats.push_back({L"Jpeg 2000",						L"jp2"					});
	Formats.push_back({L"Interlaced Bitmap",				L"lbm"					});
	Formats.push_back({L"Homeworld texture",				L"lif"					});
	Formats.push_back({L"Half-Life Model",					L"mdl"					});
	Formats.push_back({L"MPEG-1 Audio Layer 3",				L"mp3"					});
	Formats.push_back({L"Palette",							L"pal"					});
	Formats.push_back({L"Kodak PhotoCD",					L"pcd"					});
	Formats.push_back({L"ZSoft PCX",						L"pcx"					});
	Formats.push_back({L"Softimage PIC",					L"pic"					});
	Formats.push_back({L"Portable Network Graphics",		L"png"					});
	Formats.push_back({L"Portable Anymap",					L"pbm pgm pnm pnm"		});
	Formats.push_back({L"Alias | Wavefront",				L"pix"					});
	Formats.push_back({L"Adobe PhotoShop",					L"psd"					});
	Formats.push_back({L"PaintShop Pro",					L"psp"					});
	Formats.push_back({L"Pixar",							L"pxr"					});
	Formats.push_back({L"Raw data",							L"raw"					});
	Formats.push_back({L"Homeworld 2 Texture",				L"rot"					});
	Formats.push_back({L"Silicon Graphics",					L"sgi bw rgb rgba"		});
	Formats.push_back({L"Creative Assembly Texture",		L"texture"				});
	Formats.push_back({L"Truevision Targa",					L"tga"					});
	Formats.push_back({L"Tagged Image File Format",			L"tif"					});
	Formats.push_back({L"Gamecube Texture",					L"tpl"					});
	Formats.push_back({L"Unreal Texture",					L"utx"					});
	Formats.push_back({L"Quake 2 Texture",					L"wal"					});
	Formats.push_back({L"Valve Texture Format",				L"vtf"					});
	Formats.push_back({L"HD Photo",							L"wdp hdp"				});
	Formats.push_back({L"X Pixel Map",						L"xpm"					});

	//Formats.Sort([](auto & a, auto & b){ return a.first < b.first; });
	//Doom graphics 
}

CTextureFactory::~CTextureFactory()
{			
}

CTexture * CTextureFactory::CreateTexture()
{
	CTexture * t = new CTexture(Level, GraphicEngine);
	return t;
}

bool CTextureFactory::IsSupported(CStream * s)
{
	auto size = s->GetSize();
	void * data	= malloc((size_t)size);
	s->Read(data, size);

	assert(size <= UINT_MAX);
	
	auto f = ilDetermineTypeL(data, (ILuint)size) != IL_TYPE_UNKNOWN;

	free(data);

	return f;
}

CFloat3 CTextureFactory::GetDimension(CStream * s)
{
	auto size = s->GetSize();

	assert(size <= UINT_MAX);

	void * data	= _malloca((size_t)size);
	s->Read(data, size);

	ILuint handle;
	ilGenImages(1, &handle);
	ilBindImage(handle);
	ilLoadL(IL_TYPE_UNKNOWN, data, (ILuint)size);

	CFloat3 o(float(ilGetInteger(IL_IMAGE_WIDTH)), float(ilGetInteger(IL_IMAGE_HEIGHT)), float(ilGetInteger(IL_IMAGE_DEPTH)));

	ilDeleteImage(handle);

	_freea(data);
		
	return o;
}


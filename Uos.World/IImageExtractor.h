#pragma once

namespace uos
{
	auto constexpr IMAGE_EXTRACTOR_PROTOCOL = L"Uos.ImageExtractor";

	struct CGetIconMaterialJob
	{
		IType *									Requester = null;
		CString									Path;
		std::function<void(CMaterial *)>		Done;
		std::function<void()>					ImageReady;
		std::function<void()>					IconReady;
		CThread *								Thread = null;
		CImage *								Image = null;
		CMaterial *								Material = null;
		SHFILEINFOW								Sfi = {0};
	};

	class IImageExtractor : public virtual IProtocol
	{
		public:
			//virtual CImage *							GetIcon(CUol & u, int wh)=0;
			//virtual CTexture *							GetIconTexture(CUol & u, int wh)=0;
			virtual CGetIconMaterialJob *				GetIconMaterial(IType * r, CUrl & u, int wh)=0;

			virtual ~IImageExtractor(){}
	};
}
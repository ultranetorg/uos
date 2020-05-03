#pragma once
#include "Rectangle.h"
#include "VwmImporter.h"

namespace uos
{
	class CVwmElement : public CRectangle
	{
		public:
			CElement *								Content;

			UOS_RTTI
			CVwmElement(CWorldLevel * l, IDirectory * d, CString const & nodename, const CString & name = GetClassName()) : CRectangle(l, name)
			{
				CVwmImporter importer(l);
				importer.Import(d);

				auto p = importer.GetMetadata()->One(L"Node")->Find(L"Node", nodename);
				Content = importer.ImportNodeTree(p);

				AddNode(Content);

				Express(L"C",	[this](auto apply)
								{
									auto l = Climits;

									Content->Transform(CTransformation::Identity);

									auto bb = Content->Visual->GetAABB();

									auto xf = l.Smax.W > 0.f ? l.Smax.W/bb.GetWidth() : 1.f;
									auto yf = l.Smax.H > 0.f ? l.Smax.H/bb.GetHeight() : 1.f;

									auto f = min(xf, yf);

									Content->Transform(CTransformation::FromScale(f, f, f));

									bb = Content->Visual->GetAABB();

									Content->Transform(CTransformation(bb.Min * (-1.f) + CFloat3((l.Smax.W - bb.GetWidth())/2, (l.Smax.H - bb.GetHeight())/2, -bb.GetDepth()/2), CQuaternion(0, 0, 0, 1), f));

									return CSize(bb.GetWidth(), bb.GetHeight(), bb.GetDepth());
								});
			}

			~CVwmElement()
			{
				sh_free(Content);
			}

	};
}

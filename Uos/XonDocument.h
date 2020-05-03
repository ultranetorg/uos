#pragma once
#include "NativePath.h"
#include "XonReader.h"
#include "XonWriter.h"
#include "SerializableBuffer.h"
#include "Size.h"
#include "Float4.h"
#include "Float6.h"
#include "Url.h"
#include "Uxx.h"
#include "GdiRect.h"
#include "Rect.h"
#include "Transformation.h"
#include "Matrix.h"
#include "FontDefinition.h"
#include "DateTime.h"

namespace uos
{
	class UOS_LINKING CXonDocument : public CXon
	{
		public:
			void										Load(CXonDocument * t, IXonReader & w);
			void										Save(IXonWriter * w, CXonDocument * d = null);
			//CBinaryNode *								CreateBinary(CXon * p);
			
			CString										Dump();

			UOS_RTTI
			CXonDocument(CXonDocument && d);
			CXonDocument();
			virtual ~CXonDocument();
		
		protected:
			void										Merge(CXon * param, CXon * cnode);
			CXon *										Load(IXonReader & r, CXon * parent, CXon * t);
			CXon *										CreateDifference(CXon * target, CXon * p);
	};

	class UOS_LINKING CTonDocument : public CXonDocument
	{
		public:
			CTonDocument()
			{
				Value = CXonTextValue().Clone();
			}
			CTonDocument(IXonReader & r)
			{
				Value = CXonTextValue().Clone();
				Load(null, r);
			}
			CTonDocument(IXonReader & dr, IXonReader & cr)
			{
				Value = CXonTextValue().Clone();

				Load(null, dr);
				
				auto c = CTonDocument(this, cr);
				//auto cc = c.Dump();
				Merge(this, &c);
				//auto tt = Dump();
			}
			CTonDocument(CXonDocument * t, IXonReader & r)
			{
				Value = CXonTextValue().Clone();
				Load(t, r);
			}
	};

	class UOS_LINKING CBonDocument : public CXonDocument
	{
		public:
			CBonDocument()
			{
				Value = CXonBinaryValue().Clone();
			}

			CBonDocument(IXonReader & r)
			{
				Value = CXonBinaryValue().Clone();
				Load(null, r);
			}
	};
}
#pragma once

namespace uos
{
	class CRenderTarget : public virtual IType
	{
		public:
			virtual void								Apply()=0;
			virtual void								Present()=0;
			virtual void								TakeScreenshot(CString & o)=0;
			
			virtual ~CRenderTarget(){}
	};
}

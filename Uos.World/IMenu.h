#pragma once
#include "Element.h"
#include "Area.h"

namespace uos
{
	class UOS_WORLD_LINKING IMenuItem : public virtual IType
	{
		public:
			std::function<void(CInputArgs *, IMenuItem *)>	Clicked;

			UOS_RTTI
			virtual void								Highlight(CArea * a, bool e, CSize & s, CPick * p){}
	};
	
	class IMenuSection;

	class ISectionMenuItem : public virtual IMenuItem
	{
		public:
			virtual IMenuSection *						GetSection()=0;

			virtual ~ISectionMenuItem(){}
	};
	
	class IMenuSection : public virtual CShared
	{
		public:
			CEvent<IMenuSection *>						Opening;
			CEvent<IMenuSection *>						Closing;

			virtual void								Open(CArea * a, CElement * p, CPick * pick, float l, float r, float u, float d)=0;
			virtual void								Close()=0;
			virtual bool								IsOpen()=0;
			virtual void								Clear()=0;
			virtual IMenuItem *							AddItem(const CString & text)=0;
			virtual ISectionMenuItem *					AddSectionItem(const CString & text)=0;
			virtual void								AddItem(IMenuItem * item)=0;
			virtual IMenuItem *							AddSeparator()=0;

			virtual ~IMenuSection(){}
	};
	
	class IMenu
	{
		public:
			CEvent<IMenuItem *, CPick &>					Clicked;

			virtual ~IMenu(){}
	
			virtual IMenuSection *						CreateSection(const CString & name = L"MenuSection")=0; // default style section
			virtual void								SetSection(IMenuSection * m)=0;

			virtual void								Open(CPick & pick, CSize const & size = CSize::Empty)=0;
			//virtual void								Open(CViewport * vp, CFloat2 & p)=0;
			virtual void								Close()=0;
	};
}
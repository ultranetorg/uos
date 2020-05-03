#pragma once

namespace uos
{
	class CMenuItem : public CShared, public virtual IType
	{
		public:
			CString													Label;
			CRefList<CMenuItem *>									Items;
			std::function<void(CInputArgs *)>						Execute;
			std::function<void()>									Opening;
			CShared *												Meta = null;
			CUol													IconEntity;

			UOS_RTTI
			CMenuItem(CString const & label, decltype(Execute) a)
			{
				Label = label;
				Execute = a;
			}

			CMenuItem(CString const & label)
			{
				Label = label;
			}

			~CMenuItem()
			{
				sh_free(Meta);
			}

			template<class T> T * MetaAs()
			{
				return dynamic_cast<T *>(Meta);
			}
	};

}
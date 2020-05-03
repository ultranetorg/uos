#pragma once

namespace uos
{
	class UOS_LINKING CShared
	{
		private:
			int Refs=1;

		protected:
			virtual ~CShared();
	
		public:
			virtual CShared *	Take();
			virtual void		Free();
			int					GetRefs();

	};

	template<class T> void sh_free(T *& p)
	{
		if(p)
		{
			p->Free();
			p = null;
		}
	}

	template<class H, class P> P sh_assign(H h, P p)
	{
		if(h != null && h != p)
		{
			dynamic_cast<CShared *>(h)->Free();
			h = null;
		}

		if(h == p)
		{
			return h;
		}

		if(p != null)
		{
			p->Take();
			return p;
		}
		else
			return null;
	}

	template<class T> class sh_new
	{
		public:
			T * v;

			template<class ... A> sh_new(A ... a)
			{
				v = new T(std::forward<A>(a)...);
			}

			~sh_new()
			{
				v->Free();
			}

			operator T * ()
			{
				return v;
			}
	};

}
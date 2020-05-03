#pragma once

namespace uos
{
	template<class T> struct CSpaceBinding
	{
		T *					Space = null;
		CViewport *			Viewport = null;
		CList<CString>		Tags;

		bool Match(CSpaceBinding const & k)
		{
			//if(k.Space && Space && k.Space != Space)
			//{
			//	return false;
			//}

			if(k.Viewport && Viewport && k.Viewport == Viewport)
			{
				return true;
			}

			if(!k.Tags.empty() && !Tags.empty() && Tags.Has( [&k](auto & i){ return k.Tags.Contains(i); } ))
			{
				return true;
			}

			return false;
		}
	};

	template<class T> class CSpaceCollection : public CList<CSpaceBinding<T>>
	{
		public:
			CSpaceBinding<T> & Match(CViewport * vp)
			{
				return Find([vp](auto & i){ return i.Viewport == vp; });
			}

			CSpaceBinding<T> & Match(CSpaceBinding<T> & k)
			{
				return Find([&k](auto & i){ return i.Match(k); });
			}

			CSpaceBinding<T> & operator[] (CScreen * vp)
			{
				auto & k = Find([vp](auto & i){ return i.Viewport == vp; });
				
				if(!k.Space)
				{
					CSpaceBinding<T> key;
					key.Viewport = vp;
					push_back(key);
					return back();
				}
				else
					return k;
			}

			void Remove(T * s)
			{
				auto i = Findi([s](auto & i){ return i.Space == s; });
				erase(i);
			}
	};
}

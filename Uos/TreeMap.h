#pragma once
#include "List.h"


namespace uos
{
	template<class Tk, class Tv> class CMap : public std::map<Tk, Tv>
	{
		public:
			static std::pair<const Tk, Tv> Default;

			CMap(){}
			CMap(std::initializer_list<std::pair<const Tk, Tv>> l) : std::map<Tk, Tv>(l){}

			template<class Iter> CMap(Iter f, Iter l) : std::map<Tk, Tv>(f, l){}

			inline bool Contains(const Tk & k) const
			{
				return find(k) != std::map<Tk, Tv>::end();
			}

			inline bool Contains(const Tk & k, const Tk & v) const
			{
				auto i = find(k);

				if(i != end())
				{
					return i->second == v;
				}

				return false;
			}
			
			template<class Pred> bool Has(const Pred p)
			{
				return std::find_if(begin(), end(), p) != end();
			}

			inline void Remove(const Tk & v)
			{
				erase(find(v));
			}

			Tv & operator ()(Tk const & k) const
			{
				auto & i = find(k);
				
				if(i != end())
				{
					return (Tv &)i->second;
				}
				
				throw CException(HERE, L"Key not found");
			}

			template<class Pred> int Count(const Pred p)
			{
				int n = 0;
				for(auto & i : *this)
				{
					if(p(i))
					{
						n++;
					}
				}
				return n;
			}

			template<typename T, typename Pred> CList<T> Select(Pred p)
			{
				CList<T> o;
				for(auto & i : *this)
				{
					o.push_back(p(i));
				}
				return o;
			}

			template<class Pred> std::pair<const Tk, Tv> & Find(Pred p)
			{
				auto i = std::find_if(begin(), end(), p);
				if(i != end())
					return *i;
				else
					return Default;
			}
	};

	template <typename K, typename V> std::pair<const K, V> CMap<K, V>::Default = std::pair<K, V>();

}

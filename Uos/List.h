#pragma once

namespace uos
{
	#undef CList

	template<class T> class CArray;

	template<class T> class CList : public std::list<T>
	{
		public:
			void Add(CList<T> & a)
			{
				insert(end(), a.begin(), a.end());
			}

			void AddBack(T & e)
			{
				push_back(e);
			}

			void AddFront(T & a)
			{
				push_front(a);
				//insert(begin(), a.begin(), a.end());
			}

			template<class L> void AddFrontMany(L & a)
			{
				insert(begin(), a.begin(), a.end());
			}

			template<class Pred> void Do(const Pred p)
			{
				for(auto i : *this)
				{
					p(i);
				}
			}
			
			template<class Pred> bool Has(const Pred p)
			{
				return std::find_if(begin(), end(), p) != end();
			}
			
			bool Contains(const T & v) const
			{
				return std::find(begin(), end(), v) != end();
			}

			void Remove(const T& v)
			{
				auto i = std::find(begin(), end(), v);
				if(i != end())
					erase(i);
			}

			void RemoveAll(const T & v)
			{
				remove(v);
			}

			template<class Pred> void RemoveIf(Pred p)
			{
				auto e = std::remove_if(begin(), end(), p);
				erase(e, end());
			}

			template<class Tnew> CList<Tnew> Cast()
			{
				return CList<Tnew>(begin(), end());
			}

			template<class Tnew> CList<Tnew> DynamicCast()
			{
				CList<Tnew> o;
				for(auto e : *this)
				{
					o.push_back(dynamic_cast<Tnew>(e));
				}
				return o;
			}

			template<class Tnew> CList<Tnew> Only()
			{
				CList<Tnew> o;
				for(auto e : *this)
				{
					if(dynamic_cast<Tnew>(e) != null)
					{
						o.push_back(dynamic_cast<Tnew>(e));
					}
				}
				return o;
			}

			CArray<T> ToArray()
			{
				return CArray<T>(begin(), end());
			}

			template<class N> CArray<N> ToArray()
			{
				return CArray<N>(begin(), end());
			}

			template<class Pred> typename std::list<T>::iterator Findi(Pred p)
			{
				return std::find_if(begin(), end(), p);
			}

			T & First()
			{
				return !empty() ? front() : Default;
			}

			T & Last()
			{
				return !empty() ? back() : Default;
			}

			T & Previous(const T & e)
			{
				if(empty() || front() == e)
				{
					return Default;
				}

				auto i = ++begin();
				auto j = begin();

				while(i != end())
				{
					if(*i == e)
					{
						return *j;
					}

					i++;
					j++;
				}

				return Default;
			}

			template<class Equal, class Where> T & PreviousAnd(Equal eq, Where w)
			{
				if(empty() || eq(front()))
				{
					return Default;
				}

				auto i = ++begin();
				auto j = begin();

				while(i != end())
				{
					if(eq(*i) && w(*j))
					{
						return *j;
					}

					i++;
					j++;
				}

				return Default;
			}

			template<class Equal, class Where> T & NextAnd(Equal eq, Where w)
			{
				if(empty() || eq(front()))
				{
					return Default;
				}

				auto i = begin();
				auto j = ++begin();

				while(j != end())
				{
					if(eq(*i) && w(*j))
					{
						return *j;
					}

					i++;
					j++;
				}

				return Default;
			}

			template<class Pred> T & Find(Pred p)
			{
				auto i = std::find_if(begin(), end(), p);
				if(i != end())
					return *i;
				else
					return Default;
			}

			template<class N, class Pred> N Find(Pred p)
			{
				for(auto & i : *this)
				{
					if(p(dynamic_cast<N>(i)))
					{
						return dynamic_cast<N>(i);
					}
				}

				return N();
			}

			template<class Pred> T & FindBackwards(Pred p)
			{
				auto i = std::find_if(rbegin(), rend(), p);
				if(i != end())
					return *i;
				else
					return Default;
			}

			typename std::list<T>::iterator Findi(const T & v)
			{
				return std::find(begin(), end(), v);
			}


			template<class V, class Pred> V Sum(Pred p)
			{
				V s = V();
				for(auto i=begin(); i != end(); i++)
				{
					s += p(*i);
				}
				return s;
			}
			
			template<class Pred> int Count(Pred p) const
			{
				int n = 0;
				for(auto i=begin(); i != end(); i++)
				{
					if(p(*i))
					{
						n++;
					}
				}
				return n;
			}

			template<class Pred> void Sort(Pred p)
			{
				sort(p);
			}

			template<class Pred> CList<T> Where(Pred p)
			{
				CList<T> out;
				std::copy_if(begin(), end(), std::back_inserter(out), p);
				return out;
			}

			template<class P> T Max(P pred)
			{
				return *std::max_element(begin(), end(), pred);
			}

			template<class P> T Min(P pred)
			{
				return *std::min_element(begin(), end(), pred);
			}

			template<class N, class P> size_t GetHeirarchicalMax(N getnodes, P get, int tabsize = 2, int tab = 0)
			{
				size_t max = 0;
				for(auto i : *this)
				{
					auto v = get(i);
					if(v + tab > max)
					{
						max = v + tab;
					}
					
					auto nmax = getnodes(i)->GetHeirarchicalMax(getnodes, get, tabsize, tab + tabsize);
					
					if(nmax > max)
					{
						max = nmax;
					}
				}
				return max;
			}

			int GetIndex(T & a)
			{
				int n = 0;
				auto i = begin();
				while(i != end())
				{
					if(a == *i)
					{
						return n;
					}
					i++;
					n++;
				}
				return -1;
			}

			template<typename T, typename Pred> CList<T> Select(Pred p)
			{
				CList<T> o;
				for(auto i : *this)
				{
					o.push_back(p(i));
				}
				return o;
			}

			CList(){}

			CList(std::initializer_list<T> l) : std::list<T>(l)
			{

			}

			template<class Iter> CList(Iter f, Iter l) : std::list<T>(f, l)
			{
			}

		protected:
			static T Default;

	};

	template <typename T> T CList<T>::Default = T();
}

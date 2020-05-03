#pragma once

namespace uos
{
	template<class T> class CRefList : protected CList<T>
	{
		public:
			using CList<T>::Find;
			using CList<T>::Findi;
			using CList<T>::Where;
			using CList<T>::Sum;
			using CList<T>::Has;
			using CList<T>::iterator;
			using CList<T>::back;
			using CList<T>::front;
			using CList<T>::begin;
			using CList<T>::end;
			using CList<T>::rbegin;
			using CList<T>::rend;
			using CList<T>::Sort;
			using CList<T>::First;
			using CList<T>::GetIndex;
			using CList<T>::erase;
			using CList<T>::Min;
			using CList<T>::Max;
			using CList<T>::Contains;
			using CList<T>::Do;
			using CList<T>::Previous;

			operator CList<T> () const
			{
				return *this;
			}

			CRefList &  operator = (const CRefList & a)
			{
				Clear();
				AddMany(a);

				return *this;
			}

			CRefList() : CList<T>()
			{
			}

			CRefList(CRefList<T> && l) : CList<T>(l.begin(), l.end())
			{
				l.CList<T>::clear();
			}

			template<class C> CRefList(C & c)
			{
				for(auto i : c)
				{
					Add(i);
				}
			}

			CRefList(const CRefList<T> & l)
			{
				for(auto i : l)
				{
					Add(i);
				}
			}
			
			CRefList(std::initializer_list<T> l) : CList<T>(l.begin(), l.end())
			{
				for(auto i : l)
				{
					i->Take();
				}
			}

			inline void Insert(T e, typename CList<T>::iterator i)
			{
				e->Take();
				CList<T>::insert(i, e);
			}

			inline CRefList<T> Clone()
			{
				return *this;
			}

			inline bool Empty()
			{
				return empty();
			}

			inline T & Front()
			{
				return front();
			}
			
			inline void Add(T e)
			{
				CList<T>::push_back(e);
				e->Take();
			}

			inline void AddBack(T e)
			{
				CList<T>::push_back(e);
				e->Take();
			}

			inline void AddFront(T e)
			{
				CList<T>::push_front(e);
				e->Take();
			}
			
			inline void AddNew(T e)
			{
				CList<T>::push_back(e);
			}
			
			template<class C> void AddMany(C & c)
			{
				for(auto i : c)
				{
					Add(i);
				}
			}

			inline void Remove(T e)
			{
				CList<T>::Remove(e);
				e->Free();
			}

			inline void Remove(typename CList<T>::iterator i)
			{
				(*i)->Free();
				erase(i);
			}

/*
			iterator begin()
			{
				return CList<T>::begin();
			}

			const_iterator begin() const
			{
				return CList<T>::begin();
			}

			iterator end()
			{
				return CList<T>::end();
			}

			const_iterator end() const
			{
				return CList<T>::end();
			}*/
		
			int Size()
			{
				return (int)size();
			}

			void Clear()
			{
				for(auto i : *this)
				{
					i->Free();
				}
				clear();
			}

			~CRefList()
			{
				for(auto i : *this)
				{
					i->Free();
				}
			}
		
	};
}

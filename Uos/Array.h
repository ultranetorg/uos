#pragma once
//#include "ISerializable.h"

namespace uos
{
	#undef CArray

	template<class T> class CArray : public std::vector<T>
	{
		public:
			void Add(CArray & a)
			{
				insert(end(), a.begin(), a.end());
			}

			void AddFront(CArray & a)
			{
				insert(begin(), a.begin(), a.end());
			}

			template<class Pred> T & Find(Pred p)
			{
				auto i = std::find_if(begin(), end(), p);
				return i != end() ? *i : Default;
			}

			template<class Pred> void Remove(Pred p)
			{
				erase(std::remove_if(begin(), end(), p), end());
			}

			template<class Pred>  typename std::vector<T>::iterator Findi(Pred p)
			{
				return std::find_if(begin(), end(), p);
			}

			typename std::vector<T>::iterator Findi(const T & v)
			{
				return std::find(begin(), end(), v);
			}

			template<class Pred> void ForEach(Pred p)
			{
				std::for_each(begin(), end(), p);
			}

			template<class Result, class Pred> Result Sum(Pred p)
			{
				Result o = 0;
				for(auto e : *this)
				{
					o += p(e);
				}
				return o;
			}
			
			T Sum()
			{
				T o = 0;
				for(auto e : *this)
				{
					o = o + e;
				}
				return o;
			}

			template<class Pred> bool Has(Pred p)
			{
				return std::find_if(begin(), end(), p) != end();
			}

			bool Contain(const T & v)
			{
				return std::find(begin(), end(), v) != end();
			}

			void Remove(const T & v)
			{
				//boost::algorithm::erase_first(*this, [&v](T & i){ return i == v; });
				erase(std::find(begin(), end(), v));
			}

			template<class Pred> CArray<T> Where(Pred p)
			{
				CArray<T> out;
				std::copy_if(begin(), end(), std::back_inserter(out), p);
				return out;
			}

			template<class F, class Pred> CArray<F> Select(Pred p)
			{
				CArray<F> out;
				for(auto & i : *this)
				{
					out.push_back(p(i));
				}
				return out;
			}

			template<class Pred> void Sort(Pred p)
			{
				std::sort(begin(), end(), p);
			}

			template<class T> CArray<T> Cast()
			{
				return CArray<T>(begin(), end());
			}

			template<class Tnew> CArray<Tnew *> DynamicCast()
			{
				CArray<Tnew *> o;
				for(auto e : *this)
				{
					o.push_back(dynamic_cast<Tnew *>(e));
				}
				return o;
			}

			//CList<T> ToList()
			//{
			//	return CList<T>(begin(), end());
			//}
			//
			//template<class N> CList<N> ToList()
			//{
			//	return CList<N>(begin(), end());
			//}
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

			template<class P> T Max(P pred)
			{
				return *std::max_element(begin(), end(), [&pred](auto & a, auto & b){ return pred(a) < pred(b); });
			}

			T Max()
			{
				return *std::max_element(begin(), end());
			}

			T Min()
			{
				return *std::min_element(begin(), end());
			}

			int Count()
			{
				return (int)size();
			}

			CArray()
			{
			}

			CArray(std::initializer_list<T> l) : std::vector<T>(l)
			{

			}

			template<class Iter> CArray(Iter f, Iter l) : std::vector<T>(f, l)
			{
			}

			CArray(size_t n) : std::vector<T>(n)
			{
			}

			CArray(typename std::vector<T>::size_type n, const typename std::vector<T>::value_type & v) : std::vector<T>(n, v) 
			{
			}

			CArray(const T * p, size_t n) : std::vector<T>(n) 
			{
				CopyMemory(data(), p, n * sizeof(T));
			}

			//CArray(CBuffer & b) : std::vector<T>(b.GetSize()/sizeof(T)) 
			//{
			//	CopyMemory(data(), b.GetData(), b.GetSize());
			//}
		protected:
			static T Default;
	};

	template <typename T> T CArray<T>::Default = T();


/*
	template<class T> class CSerializableArray : public ISerializable
	{
		public:
			std::vector<T>	Array;

			std::wstring GetTypeName()
			{
				return T().GetTypeName()+L".array";
			}

			void Read(CStream * s)
			{
				if(b.GetSize() % sizeof(T) != 0)
				{
					throw CException(HERE, L"Wrong size");
				}

				Array.resize(b.GetSize() / sizeof(T));
				memcpy(Array.data(),  b.GetData(), b.GetSize());
			}

			CBuffer ToBinary()
			{
				return CBuffer(Array.data(), size() * sizeof(T));
			}

			std::wstring Write()
			{
				return CString::Join(Array, [](T & i){ return i.ToString(); }, L"\n");
			}

			void Read(const std::wstring & v)
			{
				T t;
				auto p = CString(v).Split(L"\n");
				Array.resize(p.size());
				for(auto i = 0U; i < p.size(); i++)
				{
					t.Parse(i);
					Array[i] = t;
				}
			}

			ISerializable * Clone()
			{
				return new CArray<T>(*this);
			}


	};*/

}

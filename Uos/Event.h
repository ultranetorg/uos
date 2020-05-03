#pragma once
#include "Array.h"
#include "Exception.h"
#include "FastDelegate.h"
#include "Delegate.h"

namespace uos
{
	#define CDelegate fastdelegate::FastDelegate

	template<class ... A> class CEvent
	{
		public:
			typedef CDelegate<void(A ...)> T;

			inline void operator += (const T & p)
			{
				if(HasDelegate(p))
				{
					throw CException(HERE, L"Already subscribed");
				}			
				Functions.push_back(p);
			}

			inline void operator -= (const T & d)
			{
				auto i = Functions.Findi(d);

				if(i != Functions.end())
				{
					Functions.erase(i);
				}
			}
			
			template<class P> void operator += (P p)
			{
				Lambdas.push_back(p);
			}

			inline bool HasDelegate(const T & p)
			{
				for(auto i=0; i<Functions.Count(); i++)
				{
					if(Functions[i] == (p))
					{
						return true;
					}
				}
				return false;
			}

			inline int GetCount()
			{
				return Functions.size();
			}

			inline void operator () (A ... args)
			{
				if(!Functions.empty())
				{
					auto f = Functions;
	
					for(auto & i : f)
					{
						i(std::forward<A>(args)...);
					}
				}
				
				if(!Lambdas.empty())
				{
					auto l = Lambdas;
	
					for(auto & i : l)
					{
						i(std::forward<A>(args)...);
					}
				}
			}

			CEvent()
			{
			}

			~CEvent()
			{
			}

		protected:
			CArray<T>									Functions;
			CArray<std::function<void(A ...)>>			Lambdas;
	};
	
	template <class X, class Y, typename...Args > CDelegate<void(Args...)> CreateEventDelegate(Y* x, void (X::*func)(Args...))
	{ 
		return CDelegate<void(Args...)>(x, func);
	}

	#define ThisHandler(method) CreateEventDelegate(this, &std::remove_pointer<decltype(this)>::type::method)


}
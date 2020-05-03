#pragma once
#ifndef DELEGATE_HPP
#define DELEGATE_HPP

//#include <cassert>
//#include <memory>
//#include <new>
//#include <type_traits>
//#include <utility>

template <typename T> class Delegate;

template<class R, class ...A> class Delegate<R(A...)>
{
	using stub_ptr_type = R(*)(void*, A&&...);

	Delegate(void* const o, stub_ptr_type const m) noexcept :
		object_ptr_(o),
		stub_ptr_(m)
	{
	}

	public:
	Delegate() = default;

	Delegate(Delegate const&) = default;

	Delegate(Delegate&&) = default;

	Delegate(::std::nullptr_t const) noexcept : Delegate() { }

	template <class C, typename =
		typename ::std::enable_if < ::std::is_class<C>{} > ::type >
		explicit Delegate(C const* const o) noexcept :
		object_ptr_(const_cast<C*>(o))
	{
	}

	template <class C, typename =
		typename ::std::enable_if < ::std::is_class<C>{} > ::type >
		explicit Delegate(C const& o) noexcept :
		object_ptr_(const_cast<C*>(&o))
	{
	}

	template <class C>
	Delegate(C* const object_ptr, R(C::* const method_ptr)(A...))
	{
		*this = from(object_ptr, method_ptr);
	}

	template <class C>
	Delegate(C* const object_ptr, R(C::* const method_ptr)(A...) const)
	{
		*this = from(object_ptr, method_ptr);
	}

	template <class C>
	Delegate(C& object, R(C::* const method_ptr)(A...))
	{
		*this = from(object, method_ptr);
	}

	template <class C>
	Delegate(C const& object, R(C::* const method_ptr)(A...) const)
	{
		*this = from(object, method_ptr);
	}

	template <
		typename T,
		typename = typename ::std::enable_if <
		!::std::is_same<Delegate, typename ::std::decay<T>::type>{}
		> ::type
	>
			Delegate(T&& f) :
			store_(operator new(sizeof(typename ::std::decay<T>::type)),
				   functor_deleter<typename ::std::decay<T>::type>),
			store_size_(sizeof(typename ::std::decay<T>::type))
		{
			using functor_type = typename ::std::decay<T>::type;

			new (store_.get()) functor_type(::std::forward<T>(f));

			object_ptr_ = store_.get();

			stub_ptr_ = functor_stub<functor_type>;

			deleter_ = deleter_stub<functor_type>;
		}

		Delegate& operator=(Delegate const&) = default;

		Delegate& operator=(Delegate&&) = default;

		template <class C>
		Delegate& operator=(R(C::* const rhs)(A...))
		{
			return *this = from(static_cast<C*>(object_ptr_), rhs);
		}

		template <class C>
		Delegate& operator=(R(C::* const rhs)(A...) const)
		{
			return *this = from(static_cast<C const*>(object_ptr_), rhs);
		}

		template <
			typename T,
			typename = typename ::std::enable_if <
			!::std::is_same<Delegate, typename ::std::decay<T>::type>{}
			> ::type
		>
				Delegate& operator=(T&& f)
			{
				using functor_type = typename ::std::decay<T>::type;

				if((sizeof(functor_type) > store_size_) || store_.use_count() != 1)
				{
					store_.reset(operator new(sizeof(functor_type)),
								 functor_deleter<functor_type>);

					store_size_ = sizeof(functor_type);
				}
				else
				{
					deleter_(store_.get());
				}

				new (store_.get()) functor_type(::std::forward<T>(f));

				object_ptr_ = store_.get();

				stub_ptr_ = functor_stub<functor_type>;

				deleter_ = deleter_stub<functor_type>;

				return *this;
			}

			template <R(*const function_ptr)(A...)>
			static Delegate from() noexcept
			{
				return {nullptr, function_stub<function_ptr>};
			}

			template <class C, R(C::* const method_ptr)(A...)>
			static Delegate from(C* const object_ptr) noexcept
			{
				return {object_ptr, method_stub<C, method_ptr>};
			}

			template <class C, R(C::* const method_ptr)(A...) const>
			static Delegate from(C const* const object_ptr) noexcept
			{
				return {const_cast<C*>(object_ptr), const_method_stub<C, method_ptr>};
			}

			template <class C, R(C::* const method_ptr)(A...)>
			static Delegate from(C& object) noexcept
			{
				return {&object, method_stub<C, method_ptr>};
			}

			template <class C, R(C::* const method_ptr)(A...) const>
			static Delegate from(C const& object) noexcept
			{
				return {const_cast<C*>(&object), const_method_stub<C, method_ptr>};
			}

			template <typename T>
			static Delegate from(T&& f)
			{
				return ::std::forward<T>(f);
			}

			static Delegate from(R(*const function_ptr)(A...))
			{
				return function_ptr;
			}

			template <class C>
			using member_pair =
				::std::pair<C* const, R(C::* const)(A...)>;

			template <class C>
			using const_member_pair =
				::std::pair<C const* const, R(C::* const)(A...) const>;

			template <class C>
			static Delegate from(C* const object_ptr,
								 R(C::* const method_ptr)(A...))
			{
				return member_pair<C>(object_ptr, method_ptr);
			}

			template <class C>
			static Delegate from(C const* const object_ptr,
								 R(C::* const method_ptr)(A...) const)
			{
				return const_member_pair<C>(object_ptr, method_ptr);
			}

			template <class C>
			static Delegate from(C& object, R(C::* const method_ptr)(A...))
			{
				return member_pair<C>(&object, method_ptr);
			}

			template <class C>
			static Delegate from(C const& object,
								 R(C::* const method_ptr)(A...) const)
			{
				return const_member_pair<C>(&object, method_ptr);
			}

			void reset() { stub_ptr_ = nullptr; store_.reset(); }

			void reset_stub() noexcept { stub_ptr_ = nullptr; }

			void swap(Delegate& other) noexcept { ::std::swap(*this, other); }

			bool operator==(Delegate const& rhs) const noexcept
			{
				return (object_ptr_ == rhs.object_ptr_) && (stub_ptr_ == rhs.stub_ptr_);
			}

			bool operator!=(Delegate const& rhs) const noexcept
			{
				return !operator==(rhs);
			}

			bool operator<(Delegate const& rhs) const noexcept
			{
				return (object_ptr_ < rhs.object_ptr_) ||
					((object_ptr_ == rhs.object_ptr_) && (stub_ptr_ < rhs.stub_ptr_));
			}

			bool operator==(::std::nullptr_t const) const noexcept
			{
				return !stub_ptr_;
			}

			bool operator!=(::std::nullptr_t const) const noexcept
			{
				return stub_ptr_ != nullptr;
			}

			explicit operator bool() const noexcept { return stub_ptr_ != nullptr; }

			R operator()(A... args) const
			{
				return stub_ptr_(object_ptr_, ::std::forward<A>(args)...);
			}

	private:
	friend struct ::std::hash<Delegate>;

	using deleter_type = void(*)(void*);

	void* object_ptr_;
	stub_ptr_type stub_ptr_{};

	deleter_type deleter_;

	::std::shared_ptr<void> store_;
	::std::size_t store_size_;

	template <class T>
	static void functor_deleter(void* const p)
	{
		static_cast<T*>(p)->~T();// - does it really need this?

		operator delete(p);
	}

	template <class T>
	static void deleter_stub(void* const p)
	{
		static_cast<T*>(p)->~T();
	}

	template <R(*function_ptr)(A...)>
	static R function_stub(void* const, A&&... args)
	{
		return function_ptr(::std::forward<A>(args)...);
	}

	template <class C, R(C::*method_ptr)(A...)>
	static R method_stub(void* const object_ptr, A&&... args)
	{
		return (static_cast<C*>(object_ptr)->*method_ptr)(
			::std::forward<A>(args)...);
	}

	template <class C, R(C::*method_ptr)(A...) const>
	static R const_method_stub(void* const object_ptr, A&&... args)
	{
		return (static_cast<C const*>(object_ptr)->*method_ptr)(
			::std::forward<A>(args)...);
	}

	template <typename>
	struct is_member_pair : std::false_type { };

	template <class C>
	struct is_member_pair< ::std::pair<C* const,
		R(C::* const)(A...)> > : std::true_type
	{
	};

	template <typename>
	struct is_const_member_pair : std::false_type { };

	template <class C>
	struct is_const_member_pair< ::std::pair<C const* const,
		R(C::* const)(A...) const> > : std::true_type
	{
	};

	template <typename T>
	static typename ::std::enable_if<
		!(is_member_pair<T>::value ||
		  is_const_member_pair<T>::value),
		R
	>::type
		functor_stub(void* const object_ptr, A&&... args)
	{
		return (*static_cast<T*>(object_ptr))(::std::forward<A>(args)...);
	}

	template <typename T>
	static typename ::std::enable_if<
		is_member_pair<T>::value ||
		is_const_member_pair<T>::value,
		R
	>::type
		functor_stub(void* const object_ptr, A&&... args)
	{
		return (static_cast<T*>(object_ptr)->first->*
				static_cast<T*>(object_ptr)->second)(::std::forward<A>(args)...);
	}
};

namespace std
{
	template <typename R, typename ...A>
	struct hash<::Delegate<R(A...)> >
	{
		size_t operator()(::Delegate<R(A...)> const& d) const noexcept
		{
			auto const seed(hash<void*>()(d.object_ptr_));

			return hash<typename ::Delegate<R(A...)>::stub_ptr_type>()(
				d.stub_ptr_) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}

#endif // DELEGATE_HPP
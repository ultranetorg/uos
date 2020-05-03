#pragma once

#include <cassert>

#include <memory>

#include <new>

#include <type_traits>

#include <utility>

template <typename T> class UniversalDelegate;

template<class R, class ...A>
class UniversalDelegate<R (A...)>
{
  using stub_ptr_type = R (*)(void*, A&&...);

  UniversalDelegate(void* const o, stub_ptr_type const m) noexcept :
    object_ptr_(o),
    stub_ptr_(m)
  {
  }

public:
  UniversalDelegate() = default;

  UniversalDelegate(UniversalDelegate const&) = default;

  UniversalDelegate(UniversalDelegate&&) = default;

  UniversalDelegate(::std::nullptr_t const) noexcept : UniversalDelegate() { }

  template <class C, typename =
    typename ::std::enable_if< ::std::is_class<C>{}>::type>
  explicit UniversalDelegate(C const* const o) noexcept :
    object_ptr_(const_cast<C*>(o))
  {
  }

  template <class C, typename =
    typename ::std::enable_if< ::std::is_class<C>{}>::type>
  explicit UniversalDelegate(C const& o) noexcept :
    object_ptr_(const_cast<C*>(&o))
  {
  }

  template <class C>
  UniversalDelegate(C* const object_ptr, R (C::* const method_ptr)(A...))
  {
    *this = from(object_ptr, method_ptr);
  }

  template <class C>
  UniversalDelegate(C* const object_ptr, R (C::* const method_ptr)(A...) const)
  {
    *this = from(object_ptr, method_ptr);
  }

  template <class C>
  UniversalDelegate(C& object, R (C::* const method_ptr)(A...))
  {
    *this = from(object, method_ptr);
  }

  template <class C>
  UniversalDelegate(C const& object, R (C::* const method_ptr)(A...) const)
  {
    *this = from(object, method_ptr);
  }

  template <
    typename T,
    typename = typename ::std::enable_if<
      !::std::is_same<UniversalDelegate, typename ::std::decay<T>::type>{}
    >::type
  >
  UniversalDelegate(T&& f) :
    /*store_(operator new(sizeof(typename ::std::decay<T>::type)),
      functor_deleter<typename ::std::decay<T>::type>),*/
    store_size_(sizeof(typename ::std::decay<T>::type))
  {
    using functor_type = typename ::std::decay<T>::type;

	//store_size_ = sizeof(functor_type);

	if(store_size_ <= sizeof(object_ptr_))
	{
		object_ptr_[0] = ((void **)&f)[0];
		object_ptr_[1] = ((void **)&f)[1];
		//CopyMemory(object_ptr_, &f, store_size_);
	}
	else
	{
		object_ptr_[0] = malloc(store_size_);
		CopyMemory(object_ptr_[0], &f, store_size_);
	}
	
    stub_ptr_ = functor_stub<functor_type>;

   //deleter_ = deleter_stub<functor_type>;
  }

  UniversalDelegate& operator=(UniversalDelegate const&) = default;

  UniversalDelegate& operator=(UniversalDelegate&&) = default;

  template <class C>
  UniversalDelegate& operator=(R (C::* const rhs)(A...))
  {
    return *this = from(static_cast<C*>(object_ptr_), rhs);
  }

  template <class C>
  UniversalDelegate& operator=(R (C::* const rhs)(A...) const)
  {
    return *this = from(static_cast<C const*>(object_ptr_), rhs);
  }

  template <
    typename T,
    typename = typename ::std::enable_if<
      !::std::is_same<UniversalDelegate, typename ::std::decay<T>::type>{}
    >::type
  >
  UniversalDelegate& operator=(T&& f)
  {
    using functor_type = typename ::std::decay<T>::type;
/*

    if ((sizeof(functor_type) > store_size_) || !store_.unique())
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

    object_ptr_ = store_.get();*/

	store_size_ = sizeof(functor_type);

	if(store_size_ <= sizeof(object_ptr_))
	{
		object_ptr_[0] = ((void **)&f)[0];
		object_ptr_[1] = ((void **)&f)[1];
	}
	else
	{
		object_ptr_[0] = malloc(store_size_);
		CopyMemory(object_ptr_[0], &f, store_size_);
	}


    stub_ptr_ = functor_stub<functor_type>;

   //deleter_ = deleter_stub<functor_type>;

    return *this;
  }

  template <R (* const function_ptr)(A...)>
  static UniversalDelegate from() noexcept
  {
    return { nullptr, function_stub<function_ptr> };
  }

  template <class C, R (C::* const method_ptr)(A...)>
  static UniversalDelegate from(C* const object_ptr) noexcept
  {
    return { object_ptr, method_stub<C, method_ptr> };
  }

  template <class C, R (C::* const method_ptr)(A...) const>
  static UniversalDelegate from(C const* const object_ptr) noexcept
  {
    return { const_cast<C*>(object_ptr), const_method_stub<C, method_ptr> };
  }

  template <class C, R (C::* const method_ptr)(A...)>
  static UniversalDelegate from(C& object) noexcept
  {
    return { &object, method_stub<C, method_ptr> };
  }

  template <class C, R (C::* const method_ptr)(A...) const>
  static UniversalDelegate from(C const& object) noexcept
  {
    return { const_cast<C*>(&object), const_method_stub<C, method_ptr> };
  }

  template <typename T>
  static UniversalDelegate from(T&& f)
  {
    return ::std::forward<T>(f);
  }

  static UniversalDelegate from(R (* const function_ptr)(A...))
  {
    return function_ptr;
  }

  template <class C>
  using member_pair =
    ::std::pair<C* const, R (C::* const)(A...)>;

  template <class C>
  using const_member_pair =
    ::std::pair<C const* const, R (C::* const)(A...) const>;

  template <class C>
  static UniversalDelegate from(C* const object_ptr,
    R (C::* const method_ptr)(A...))
  {
    return member_pair<C>(object_ptr, method_ptr);
  }

  template <class C>
  static UniversalDelegate from(C const* const object_ptr,
    R (C::* const method_ptr)(A...) const)
  {
    return const_member_pair<C>(object_ptr, method_ptr);
  }

  template <class C>
  static UniversalDelegate from(C& object, R (C::* const method_ptr)(A...))
  {
    return member_pair<C>(&object, method_ptr);
  }

  template <class C>
  static UniversalDelegate from(C const& object,
    R (C::* const method_ptr)(A...) const)
  {
    return const_member_pair<C>(&object, method_ptr);
  }

  void reset() { stub_ptr_ = nullptr; store_.reset(); }

  void reset_stub() noexcept { stub_ptr_ = nullptr; }

  void swap(UniversalDelegate& other) noexcept { ::std::swap(*this, other); }

  bool operator==(UniversalDelegate const& rhs) const noexcept
  {
	//auto p = static_cast<T*>(object_ptr_)->first;
	//stub_ptr_ == 

	//auto n = memcmp(object_ptr_, rhs.object_ptr_, this->store_size_);

    return (object_ptr_ == rhs.object_ptr_) && (stub_ptr_ == rhs.stub_ptr_);
  }


  bool equals(UniversalDelegate const& rhs) const noexcept
  {
    return store_size_ == rhs.store_size_ && memcmp(object_ptr_, rhs.object_ptr_, store_size_) == 0 && (stub_ptr_ == rhs.stub_ptr_);
  }


  bool operator!=(UniversalDelegate const& rhs) const noexcept
  {
    return !operator==(rhs);
  }

  bool operator<(UniversalDelegate const& rhs) const noexcept
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
    return stub_ptr_;
  }

  explicit operator bool() const noexcept { return stub_ptr_; }

  R operator()(A... args) const
  {
//  assert(stub_ptr);
    return stub_ptr_((void *)object_ptr_, ::std::forward<A>(args)...);
  }

  ~UniversalDelegate()
  {
	if(store_size_ > sizeof(object_ptr_))
	{
		free(object_ptr_[0]);
	}
  }

private:
  friend struct ::std::hash<UniversalDelegate>;

  using deleter_type = void (*)(void*);

  void* object_ptr_[2];
  
  stub_ptr_type stub_ptr_;

  deleter_type deleter_;

//  ::std::shared_ptr<void> store_;
  ::std::size_t store_size_;

  template <class T>
  static void functor_deleter(void* const p)
  {
    static_cast<T*>(p)->~T();

    operator delete(p);
  }

  template <class T>
  static void deleter_stub(void* const p)
  {
    static_cast<T*>(p)->~T();
  }

  template <R (*function_ptr)(A...)>
  static R function_stub(void* const, A&&... args)
  {
    return function_ptr(::std::forward<A>(args)...);
  }

  template <class C, R (C::*method_ptr)(A...)>
  static R method_stub(void* const object_ptr, A&&... args)
  {
    return (static_cast<C*>(object_ptr)->*method_ptr)(
      ::std::forward<A>(args)...);
  }

  template <class C, R (C::*method_ptr)(A...) const>
  static R const_method_stub(void* const object_ptr, A&&... args)
  {
    return (static_cast<C const*>(object_ptr)->*method_ptr)(
      ::std::forward<A>(args)...);
  }

  template <typename>
  struct is_member_pair : std::false_type { };

  template <class C>
  struct is_member_pair< ::std::pair<C* const,
    R (C::* const)(A...)> > : std::true_type
  {
  };

  template <typename>
  struct is_const_member_pair : std::false_type { };

  template <class C>
  struct is_const_member_pair< ::std::pair<C const* const,
    R (C::* const)(A...) const> > : std::true_type
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
    return (*static_cast<T*>(((void **)object_ptr)[0]))(::std::forward<A>(args)...);
  }

  template <typename T>
  static typename ::std::enable_if<
    (is_member_pair<T>::value ||
    is_const_member_pair<T>::value),
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
  struct hash<::UniversalDelegate<R (A...)> >
  {
    size_t operator()(::UniversalDelegate<R (A...)> const& d) const noexcept
    {
      auto const seed(hash<void*>()(d.object_ptr_));

      return hash<typename ::UniversalDelegate<R (A...)>::stub_ptr_type>()(
        d.stub_ptr_) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  };
}




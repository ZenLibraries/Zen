#ifndef ZEN_CLONE_PTR_HPP
#define ZEN_CLONE_PTR_HPP

#include <utility>

ZEN_NAMESPACE_START

template<typename T, typename Enabler = void>
struct default_cloner;

template<typename T>
struct default_cloner<T, std::enable_if_t<std::is_fundamental_v<T>>> {

  default_cloner();

  template<typename T2>
  default_cloner(default_cloner<T2>&& other) {}

  template<typename T2>
  default_cloner(const default_cloner<T2>& other) {}

  T operator()(const T& value) {
    return value;
  }
};

template<typename T>
struct default_cloner<T*> {

  default_cloner() {}

  template<typename T2>
  default_cloner(default_cloner<T2>&& other) {}

  template<typename T2>
  default_cloner(const default_cloner<T2>& other) {}

  T* operator()(const T* value) {
    return value->clone();
  }
};

template<typename T, typename ClonerT = default_cloner<T*>>
class clone_ptr {

  template<typename T2, typename ClonerT2>
  friend class clone_ptr;

  ClonerT cloner;
  T* ptr;

public:

  clone_ptr(T* ptr, ClonerT cloner = default_cloner<T*> {}):
    cloner(cloner), ptr(ptr) {}

  clone_ptr(const clone_ptr& other):
    cloner(other.cloner), ptr(cloner(other.ptr)) {}

  clone_ptr(clone_ptr&& other):
    cloner(std::move(other.cloner)), ptr(std::move(other.ptr)) {
      other.ptr = nullptr;
    };

  clone_ptr& operator=(const clone_ptr& other) {
    cloner = other.cloner;
    ptr = cloner(other.ptr);
    return *this;
  }

  clone_ptr& operator=(clone_ptr&& other) {
    cloner = std::move(other.cloner);
    ptr = std::move(other.ptr);
    other.ptr = nullptr;
    return *this;
  }

  template<typename T2, typename ClonerT2>
  clone_ptr(const clone_ptr<T2, ClonerT2>& other):
    cloner(other.cloner), ptr(cloner(other.ptr)) {}

  template<typename T2, typename ClonerT2>
  clone_ptr(clone_ptr<T2, ClonerT2>&& other):
    cloner(std::move(other.cloner)), ptr(std::move(other.ptr)) {
      other.ptr = nullptr;
    };

  template<typename T2, typename ClonerT2>
  clone_ptr& operator=(const clone_ptr<T2, ClonerT2>& other) {
    cloner = other.cloner;
    ptr = cloner(other.ptr);
    return *this;
  }

  template<typename T2, typename ClonerT2>
  clone_ptr& operator=(clone_ptr<T2, ClonerT2>&& other) {
    cloner = std::move(other.cloner);
    ptr = std::move(other.ptr);
    other.ptr = nullptr;
    return *this;
  }

  T* get() {
    return ptr;
  }

  const T* get() const {
    return ptr;
  }

  T* release() {
    auto keep = ptr;
    ptr = nullptr;
    return keep;
  }

  T& operator*() {
    return *ptr;
  }

  const T& operator*() const {
    return *ptr;
  }

  T* operator->() {
    return ptr;
  }

  const T* operator->() const {
    return ptr;
  }

  ~clone_ptr() {
    if (ptr != nullptr) {
      delete ptr;
    }
  }

};

template<typename T, typename ...ForwardArgs>
clone_ptr<T> make_cloned(ForwardArgs&& ...args) {
  return clone_ptr<T>(new T(std::forward<ForwardArgs>(args)...));
}

ZEN_NAMESPACE_END

#endif // of #ifndef ZEN_CLONE_PTR_HPP

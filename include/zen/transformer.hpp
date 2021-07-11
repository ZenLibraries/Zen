#ifndef ZEN_TRANSFORMER_HPP
#define ZEN_TRANSFORMER_HPP

#include <iterator>
#include <sstream>
#include <type_traits>
#include <memory>
#include <optional>

#include "zen/config.hpp"
#include "zen/meta.hpp"

ZEN_NAMESPACE_START

class transformer;
class sequence_transformer;
class object_transformer;

template<typename T, typename = void>
struct has_transform_method : std::false_type {};

template<typename T>
struct has_transform_method<T,
    std::void_t<decltype(std::declval<T&>().transform(std::declval<transformer&>()))>
    > : std::true_type { };

template<typename T>
static constexpr const bool has_transform_method_v = has_transform_method<T>::value;

class transformer {
public:

  virtual void transform(bool& value) = 0;
  virtual void transform(char& value) = 0;
  virtual void transform(short& value) = 0;
  virtual void transform(int& value) = 0;
  virtual void transform(long& value) = 0;
  virtual void transform(long long& value) = 0;
  virtual void transform(unsigned char& value) = 0;
  virtual void transform(unsigned short& value) = 0;
  virtual void transform(unsigned int& value) = 0;
  virtual void transform(unsigned long& value) = 0;
  virtual void transform(unsigned long long& value) = 0;
  virtual void transform(float& value) = 0;
  virtual void transform(double& value) = 0;
  virtual void transform(std::string& value) = 0;

  virtual void start_transform_optional() = 0;
  virtual void transform_nil() = 0;
  virtual void end_transform_optional() = 0;

  virtual void start_transform_object(const std::string& tag_name) = 0;
  virtual void start_transform_field(const std::string& name) = 0;
  virtual void end_transform_field() = 0;
  virtual void end_transform_object() = 0;

  virtual void start_transform_sequence() = 0;
  virtual void transform_size(std::size_t size) = 0;
  virtual void start_transform_element() = 0;
  virtual void end_transform_element() = 0;
  virtual void end_transform_sequence() = 0;

  template<typename T>
  void transform(std::optional<T>& value);

  template<typename T1, typename T2>
  void transform(std::pair<T1, T2>& value);

  template<typename T>
  std::enable_if_t<meta::is_pointer_v<T>> transform(T& value);

  template<typename T>
  std::enable_if_t<meta::is_container_v<T>> transform(T& value);

  template<typename T>
  std::enable_if_t<has_transform_method_v<T>> transform(T& value);

  object_transformer transform_object(const std::string& tag_name);
  sequence_transformer transform_sequence(std::size_t size);

  virtual ~transformer() {}

};

class object_transformer {

  transformer& transformer;

#ifndef NDEBUG
  bool has_finalized = false;
#endif

public:

  object_transformer(class transformer& transformer):
    transformer(transformer) {}

  template<typename T>
  void transform_field(std::string name, T value) {
    transformer.start_transform_field(name);
    transformer.transform(value);
    transformer.end_transform_field();
  }

  void finalize() {
    transformer.end_transform_object();
#ifndef NDEBUG
    has_finalized = true;
#endif
  }

#ifndef NDEBUG
  ~object_transformer() {
    if (!has_finalized) {
      ZEN_PANIC("detected a missing call to zen::object_transformer::finalize()");
    }
  }
#endif

};

inline object_transformer transformer::transform_object(const std::string& tag_name) {
  start_transform_object(tag_name);
  return object_transformer(*this);
}

class sequence_transformer {

  transformer& transformer;

#ifndef NDEBUG
  bool has_finalized = false;
#endif

public:

  inline sequence_transformer(class transformer& transformer):
    transformer(transformer)  {}

  template<typename T>
  void transform(T value) {
    transformer.start_transform_element();
    transformer.transform(value);
    transformer.end_transform_element();
  }

  void finalize() {
    transformer.end_transform_sequence();
#ifndef NDEBUG
    has_finalized = true;
#endif
  }

#ifndef NDEBUG
  ~sequence_transformer() {
    if (!has_finalized) {
      ZEN_PANIC("detected a missing call to zen::sequence_transformer::finalize()");
    }
  }
#endif

};

inline sequence_transformer transformer::transform_sequence(std::size_t size) {
  start_transform_sequence();
  transform_size(size);
  return sequence_transformer(*this);
}

template<typename T>
void transformer::transform(std::optional<T>& value) {
  start_transform_optional();
  if (value.has_value()) {
    transform_nil();
  } else {
    transform(*value);
  }
  end_transform_optional();
}

template<typename T1, typename T2>
void transformer::transform(std::pair<T1, T2>& value) {
  start_transform_sequence();
  start_transform_element();
  transform(value.first);
  end_transform_element();
  start_transform_element();
  transform(value.second);
  end_transform_element();
  end_transform_sequence();
}

template<typename T>
std::enable_if_t<meta::is_container_v<T>> transformer::transform(T& value) {
  start_transform_sequence();
  transform_size(value.size());
  for (const auto& element: value) {
    start_transform_element();
    transform(element);
    end_transform_element();
  }
  end_transform_sequence();
}

template<typename T>
std::enable_if_t<meta::is_pointer_v<T>> transformer::transform(T& value) {
  start_transform_optional();
  if (value == nullptr) {
    transform_nil();
  } else {
    transform(*value);
  }
  end_transform_optional();
}

template<typename T>
std::enable_if_t<has_transform_method_v<T>> transformer::transform(T& value) {
  value.transform(*this);
}

template<typename T>
void decode(transformer& decoder, T& value) {
  // FIXME The object tag is set to the empty string
  auto s = decoder.transform_object("");
  value.transform_fields(s);
  s.finalize();
}



ZEN_NAMESPACE_END

#endif // of #ifndef ZEN_TRANSFORMER_HPP

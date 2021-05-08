#ifndef ZEN_ENCODER_HPP
#define ZEN_ENCODER_HPP

#include <iterator>
#include <sstream>
#include <type_traits>
#include <memory>
#include <optional>

#include "zen/meta.hpp"

ZEN_NAMESPACE_START

class encoder;
class sequence_encoder;
class struct_encoder;

template<typename T, typename = void>
struct has_encode_method : std::false_type {};

template<typename T>
struct has_encode_method<T,
    std::void_t<decltype(std::declval<const T&>().encode(std::declval<encoder&>()))>
    > : std::true_type { };

template<typename T>
static constexpr const bool has_encode_method_v = has_encode_method<T>::value;

class encoder {
public:

  virtual void encode(bool value) = 0;
  virtual void encode(char value) = 0;
  virtual void encode(short value) = 0;
  virtual void encode(int value) = 0;
  virtual void encode(long value) = 0;
  virtual void encode(long long value) = 0;
  virtual void encode(unsigned char value) = 0;
  virtual void encode(unsigned short value) = 0;
  virtual void encode(unsigned int value) = 0;
  virtual void encode(unsigned long value) = 0;
  virtual void encode(unsigned long long value) = 0;
  virtual void encode(float value) = 0;
  virtual void encode(double value) = 0;
  virtual void encode(const std::string_view& value) = 0;

  virtual void start_encode_optional() = 0;
  virtual void encode_nil() = 0;
  virtual void end_encode_optional() = 0;

  virtual void start_encode_struct(std::string tag_name) = 0;
  virtual void start_encode_field(std::string name) = 0;
  virtual void end_encode_field() = 0;
  virtual void end_encode_struct() = 0;

  virtual void start_encode_sequence() = 0;
  virtual void encode_size(std::size_t size) = 0;
  virtual void start_encode_element() = 0;
  virtual void end_encode_element() = 0;
  virtual void end_encode_sequence() = 0;

  inline void encode(const std::string& str) {
    std::string_view view(str);
    encode(view);
  }

  template<typename T>
  void encode(const std::optional<T>& value);

  template<typename T1, typename T2>
  void encode(const std::pair<T1, T2>& value);

  template<typename T>
  std::enable_if_t<meta::is_pointer_v<T>> encode(const T& value);

  template<typename T>
  std::enable_if_t<meta::is_container_v<T>> encode(const T& value);

  template<typename T>
  std::enable_if_t<has_encode_method_v<T>> encode(const T& value);

  struct_encoder encode_struct(std::string tag_name);
  sequence_encoder encode_sequence(std::size_t size);

  virtual ~encoder() {}

};

class struct_encoder {

  encoder& encoder;

public:

  struct_encoder(class encoder& encoder):
    encoder(encoder) {}

  template<typename T>
  void encode_field(std::string name, T value) {
    encoder.start_encode_field(name);
    encoder.encode(value);
    encoder.end_encode_field();
  }

  void finalize() {
    encoder.end_encode_struct();
  }

};

inline struct_encoder encoder::encode_struct(std::string tag_name) {
  start_encode_struct(tag_name);
  return struct_encoder(*this);
}

class sequence_encoder {

  encoder& encoder;

public:

  sequence_encoder(class encoder& encoder):
    encoder(encoder)  {}

  template<typename T>
  void encode(T value) {
    encoder.start_encode_element();
    encoder.encode(value);
    encoder.end_encode_element();
  }

  void finalize() {
    encoder.end_encode_struct();
  }

};

inline sequence_encoder encoder::encode_sequence(std::size_t size) {
  start_encode_sequence();
  encode_size(size);
  return sequence_encoder(*this);
}

template<typename T>
void encoder::encode(const std::optional<T>& value) {
  start_encode_optional();
  if (value.has_value()) {
    encode_nil();
  } else {
    encode(*value);
  }
  end_encode_optional();
}

template<typename T1, typename T2>
void encoder::encode(const std::pair<T1, T2>& value) {
  start_encode_sequence();
  start_encode_element();
  encode(value.first);
  end_encode_element();
  start_encode_element();
  encode(value.second);
  end_encode_element();
  end_encode_sequence();
}

template<typename T>
std::enable_if_t<meta::is_container_v<T>> encoder::encode(const T& value) {
  start_encode_sequence();
  encode_size(value.size());
  for (const auto& element: value) {
    start_encode_element();
    encode(element);
    end_encode_element();
  }
  end_encode_sequence();
}

template<typename T>
std::enable_if_t<meta::is_pointer_v<T>> encoder::encode(const T& value) {
  start_encode_optional();
  if (value == nullptr) {
    encode_nil();
  } else {
    encode(*value);
  }
  end_encode_optional();
}

template<typename T>
std::enable_if_t<has_encode_method_v<T>> encoder::encode(const T& value) {
  value.encode(*this);
}

struct json_encode_opts {
  std::string indentation = "";
};

std::unique_ptr<encoder> make_json_encoder(std::ostream& out, json_encode_opts opts);

template<typename T>
std::string encode_json(const T& value, json_encode_opts opts = json_encode_opts {}) {
  std::ostringstream ss;
  auto ec = make_json_encoder(ss, opts);
  ec->encode(value);
  return ss.str();
}

template<typename T>
std::string encode_json_pretty(const T& value) {
  std::ostringstream ss;
  auto ec = make_json_encoder(ss, json_encode_opts { .indentation = "  " });
  ec->encode(value);
  return ss.str();
}

ZEN_NAMESPACE_END

#endif // of #ifndef ZEN_ENCODER_HPP

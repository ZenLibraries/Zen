#ifndef ZEN_VALUE_HPP
#define ZEN_VALUE_HPP

#include <vector>
#include <memory>

#include "zen/clone_ptr.hpp"
#include "zen/config.hpp"
#include "zen/string.hpp"
#include "zen/seq_map.hpp"

ZEN_NAMESPACE_START

using bigint = long long;

using fractional = double;

enum class value_type {
  array,
  boolean,
  string,
  null,
  integer,
  fractional,
  object,
};

class value;

class null {};


class value {
public:

  using array = std::vector<value>;
  using object = seq_map<string, value>;

private:

  value_type type;

  union {
    bool b;
    bigint i;
    fractional f;
    string s;
    array a;
    object o;
  };

public:

  value() {}

  value(null):
    type(value_type::null) {}

  value(bool b):
    type(value_type::boolean), b(b) {}

  value(bigint i):
    type(value_type::integer), i(i) {}

  value(fractional f):
    type(value_type::fractional), f(f) {}

  value(object o):
    type(value_type::object), o(o) {}

  value(array a):
    type(value_type::array), a(a) {}

  value(string s):
    type(value_type::string), s(s) { }

  value(const value& other): type(other.type) {
    switch (other.type) {
      case value_type::array:
        new (&a) array(other.a);
        break;
      case value_type::object:
        new (&o) object(other.o);
        break;
      case value_type::integer:
        new (&i) bigint(other.i);
        break;
      case value_type::boolean:
        b = other.b;
        break;
      case value_type::null:
        break;
      case value_type::string:
        new (&s) string(other.s);
        break;
      case value_type::fractional:
        new (&f) fractional(other.f);
        break;
    }
  }

  value(value&& other): type(std::move(other.type)) {
    switch (other.type) {
      case value_type::array:
        new (&a) array(std::move(other.a));
        break;
      case value_type::object:
        new (&o) object(std::move(other.o));
        break;
      case value_type::integer:
        new (&i) bigint(std::move(other.i));
        break;
      case value_type::boolean:
        b = other.b;
        break;
      case value_type::null:
        break;
      case value_type::string:
        new (&s) string(std::move(other.s));
        break;
      case value_type::fractional:
        new (&f) fractional(std::move(other.f));
        break;
    }
    other.type = value_type::null;
  }

  value& operator=(const value& other) {
    type = other.type;
    switch (other.type) {
      case value_type::array:
        new (&a) array(other.a);
        break;
      case value_type::object:
        new (&o) object(other.o);
        break;
      case value_type::integer:
        new (&i) bigint(other.i);
        break;
      case value_type::boolean:
        b = other.b;
        break;
      case value_type::null:
        break;
      case value_type::string:
        new (&s) string(other.s);
        break;
      case value_type::fractional:
        new (&f) fractional(other.f);
        break;
    }
    return *this;
  }

  value& operator=(value&& other) {
    type = std::move(other.type);
    switch (other.type) {
      case value_type::array:
        new (&a) array(std::move(other.a));
        break;
      case value_type::object:
        new (&o) object(std::move(other.o));
        break;
      case value_type::integer:
        new (&i) bigint(std::move(other.i));
        break;
      case value_type::boolean:
        b = other.b;
        break;
      case value_type::null:
        break;
      case value_type::string:
        new (&s) string(std::move(other.s));
        break;
      case value_type::fractional:
        new (&f) fractional(std::move(other.f));
        break;
    }
    other.type = value_type::null;
    return *this;
  }

  inline ~value() {
    switch (type) {
      case value_type::string:
        s.~string();
        break;
      case value_type::fractional:
        f.~fractional();
        break;
      case value_type::array:
        a.~array();
        break;
      case value_type::object:
        o.~object();
        break;
      case value_type::boolean:
        break;
      case value_type::integer:
        i.~bigint();
        break;
      case value_type::null:
        break;
    }
  }

  inline value_type get_type() const noexcept {
    return type;
  }

  inline bool& as_boolean() {
    ZEN_ASSERT(type == value_type::boolean);
    return b;
  }

  inline const bool& as_boolean() const {
    ZEN_ASSERT(type == value_type::boolean);
    return b;
  }

  inline string& as_string() {
    ZEN_ASSERT(type == value_type::string);
    return s;
  }

  inline const string& as_string() const {
    ZEN_ASSERT(type == value_type::string);
    return s;
  }

  inline bigint& as_integer() {
    ZEN_ASSERT(type == value_type::integer);
    return i;
  }

  inline const bigint& as_integer() const {
    ZEN_ASSERT(type == value_type::integer);
    return i;
  }

  inline fractional& as_fractional() {
    ZEN_ASSERT(type == value_type::fractional);
    return f;
  }

  inline const fractional& as_fractional() const {
    ZEN_ASSERT(type == value_type::fractional);
    return f;
  }

  inline array& as_array() {
    ZEN_ASSERT(type == value_type::array);
    return a;
  }

  inline const array& as_array() const {
    ZEN_ASSERT(type == value_type::array);
    return a;
  }

  inline object& as_object() {
    ZEN_ASSERT(type == value_type::object);
    return o;
  }

  inline const object& as_object() const {
    ZEN_ASSERT(type == value_type::object);
    return o;
  }

  inline bool is_true() const noexcept {
    return type == value_type::boolean && b;
  }

  inline bool is_false() const noexcept {
    return type == value_type::boolean && !b;
  }

  inline bool is_boolean() const noexcept {
    return type == value_type::boolean;
  }

  inline bool is_integer() const noexcept {
    return type == value_type::integer;
  }

  inline bool is_fractional() const noexcept {
    return type == value_type::fractional;
  }

  inline bool is_null() const noexcept {
    return type == value_type::null;
  }

  inline bool is_string() const noexcept {
    return type == value_type::string;
  }

  inline bool is_object() const noexcept {
    return type == value_type::object;
  }

  inline bool is_array() const noexcept {
    return type == value_type::array;
  }

};

using array = value::array;
using object = value::object;


ZEN_NAMESPACE_END

#endif // of #ifndef ZEN_VALUE_HPP

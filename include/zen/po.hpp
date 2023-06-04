
#pragma once

#include <algorithm>
#include <variant>
#include <string>
#include <optional>
#include <functional>
#include <unordered_map>
#include <any>
#include <vector>
#include <memory>
#include <typeindex>

#include "zen/clone_ptr.hpp"
#include "zen/config.hpp"
#include "zen/range.hpp"
#include "zen/either.hpp"

ZEN_NAMESPACE_START

namespace po {

  struct posarg {
    std::string _name;
    int _arity;
  };

  template<typename T>
  class flag {

    friend class command;

    std::string name;
    std::optional<std::string> description;
    std::size_t min_count = 1;
    std::size_t max_count = 1;

  public:

    flag(std::string name, std::optional<std::string> description = {}):
      name(name), description(description) {}

    flag& optional() {
      min_count = 0;
      return *this;
    }

    flag& required() {
      min_count = std::max(static_cast<std::size_t>(1), min_count);
      return *this;
    }

  };

  struct unrecognised_flag_error {

    std::string flag_name;

    void display(std::ostream& out) const {
      out << "the flag '" << flag_name << "' was not recognised by any of the (sub)commands.";
    }

  };

  struct unsupported_type_error {

    std::string flag_name;

    void display(std::ostream& out) const {
      out << "trying to store a flag value in a type that cannot be parsed."; 
    }

  };

  struct flag_value_missing_error {

    std::string flag_name;

    void display(std::ostream& out) const {
      out << "no value provided for flag '" << flag_name << "'.";
    }

  };

  struct excess_positional_arg_error {

    std::size_t i;
    std::string arg;

    void display(std::ostream& out) const {
      out << "excess positional argument '" << arg << "' found.";
    }

  };

  struct command_not_found_error {
 
    std::string actual;

    void display(std::ostream& out) const {
      out << "the command '" << actual << "' was not found.";
    }

  };

  struct invalid_argument_error {

    std::string_view actual;

    invalid_argument_error(std::string_view actual):
      actual(actual) {}

    void display(std::ostream& out) const {
      out << "the argument '" << actual << "' could not be parsed.";
    }

  };

  struct missing_pos_arg_error {

    posarg expected;

    missing_pos_arg_error(posarg expected):
      expected(expected) {}

    void display(std::ostream& out) const {
      out << "a positional argument for " << expected._name << " was missing.";
    }

  };

  class error {

    using storage_t = std::variant<
      invalid_argument_error,
      missing_pos_arg_error,
      command_not_found_error,
      excess_positional_arg_error,
      flag_value_missing_error,
      unsupported_type_error,
      unrecognised_flag_error
    >;

    storage_t storage;

  public:

    template<typename T>
    error(T inner):
      storage(inner) {}

    void display(std::ostream& out) const {
      // TODO can probably do some metaprogramming to automate this
      switch (storage.index()) {
        case 0:
          std::get<0>(storage).display(out);
          break;
        case 1:
          std::get<1>(storage).display(out);
          break;
        case 2:
          std::get<2>(storage).display(out);
          break;
        case 3:
          std::get<3>(storage).display(out);
          break;
        case 4:
          std::get<4>(storage).display(out);
          break;
        case 5:
          std::get<5>(storage).display(out);
          break;
        case 6:
          std::get<6>(storage).display(out);
          break;
      }
    }

    template<typename T>
    bool is() const {
      return std::holds_alternative<T>(storage);
    }

    template<typename T>
    T& as() {
      return std::get<T>(storage);
    }

    template<typename T>
    const T& as() const {
      return std::get<T>(storage);
    }

  };

  template<typename T>
  using result = either<error, T>;

  class match {

    friend class program;

    std::unordered_map<std::string, std::any> _flags;
    std::vector<std::string> _pos_args;
    std::optional<std::tuple<std::string, clone_ptr<match>>> _subcommand = {};

    void add_flag(std::string name, std::any value) {
      _flags.emplace(name, value);
    }

    void add_pos_arg(std::string arg) {
      _pos_args.push_back(arg);
    }

  public:

    inline match():
      _subcommand({}) {}

    inline match(
      std::unordered_map<std::string, std::any> _flags,
      std::vector<std::string> _pos_args,
      std::optional<std::tuple<std::string, clone_ptr<match>>> _subcommand
    ): _flags(_flags), _pos_args(_pos_args), _subcommand(_subcommand) {}

    match* clone() const {
      return new match { _flags, _pos_args, _subcommand };
    }

    std::size_t count_flags() const {
      return _flags.size();
    }

    bool has_flag(const std::string& name) const {
      return _flags.count(name);
    }

    template<typename T>
    std::optional<T> get_flag(const std::string& name) const {
      auto match = _flags.find(name);
      if  (match == _flags.end()) {
        return {};
      }
      return std::any_cast<T>(match->second);
    }

    std::size_t count_pos_args() const {
      return _pos_args.size();
    }

    std::string get_pos_arg(std::size_t i) {
      return _pos_args[i];
    }

    auto get_pos_args() const {
      return make_iterator_range(_pos_args.cbegin(), _pos_args.cend());
    }

    bool has_subcommand() const noexcept {
      return _subcommand.has_value();
    }

    std::tuple<std::string, clone_ptr<match>>& subcommand() {
      ZEN_ASSERT(_subcommand);
      return *_subcommand;
    }

  };

  using command_callback_t = std::function<int(const match&)>;

  static constexpr const int opt = -3;
  static constexpr const int some = -2;
  static constexpr const int many = -1;

  struct _flag_info {
    std::optional<std::string> description;
    std::type_index type;
    std::size_t min_count;
    std::size_t max_count;
  };

  class command {

    friend class program;

  protected:

    std::string _name;
    std::optional<std::string> _description;
    std::unordered_map<std::string, _flag_info> _flags;
    std::vector<command> _subcommands;
    std::vector<posarg> _pos_args;
    bool _is_fallback = false;
    std::optional<command_callback_t> _callback;

  public:

    inline command(std::string name, std::optional<std::string> description = {}):
      _name(name), _description(description) {}

    command& description(std::string description) {
      _description = description;
      return *this;
    }

    template<typename T>
    command& flag(flag<T> fl) {
      _flags.emplace(fl.name, _flag_info { fl.description, typeid(T), fl.min_count, fl.max_count });
      return *this;
    }

    command& action(command_callback_t callback) {
      _callback = callback;
      return *this;
    }

    command& fallback() {
      _is_fallback = true;
      return *this;
    }

    command& subcommand(command cmd) {
      _subcommands.push_back(cmd);
      return *this;
    }

    command& pos_arg(std::string name, int arity = 1) {
      _pos_args.push_back(posarg(name, arity));
      return *this;
    }

  };

  inline bool starts_with(const std::string_view& str, const std::string_view& needle) {
    auto it1 = str.begin();
    auto it2 = needle.begin();
    for (;;) {
      if (it2 == needle.end()) {
        return true;
      }
      if (it1 == str.end() || *it1 != *it2) {
        return false;
      }
      ++it1;
      ++it2;
    }
  }

  class program {

    command _command;

  public:

    inline program(std::string name, std::optional<std::string> description = {}):
      _command(name, description) {}

    program& description(std::string description) {
      _command.description(description);
      return *this;
    }

    program& subcommand(command cmd) {
      _command.subcommand(cmd);
      return *this;
    }

    template<typename T>
    program& flag(flag<T> fl) {
      _command.flag(fl);
      return *this;
    }

    program& pos_arg(std::string name, int arity = 1) {
      _command.pos_arg(name, arity);
      return *this;
    }

    result<match> parse_args(std::vector<std::string_view> argv);

    result<match> parse_args(int argc, const char** argv);

  };

}

ZEN_NAMESPACE_END

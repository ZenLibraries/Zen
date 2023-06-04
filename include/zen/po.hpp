
#pragma once

#include <variant>
#include <string>
#include <optional>
#include <functional>
#include <unordered_map>
#include <any>
#include <vector>
#include <memory>

#include "zen/config.hpp"
#include "zen/range.hpp"
#include "zen/either.hpp"

ZEN_NAMESPACE_START

namespace po {

  struct posarg {
    std::string _name;
    int _arity;
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

    using storage_t = std::variant<invalid_argument_error, missing_pos_arg_error>;

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
      }
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

  class parse_result {

    friend class program;

    std::unordered_map<std::string, std::any> flags;
    std::vector<std::string> pos_args;

    void add_flag(std::string name, std::any value) {
      flags.emplace(name, value);
    }

    void add_pos_arg(std::string arg) {
      pos_args.push_back(arg);
    }

  public:

    std::size_t count_flags() const {
      return flags.size();
    }

    std::optional<std::any> get_flag(const std::string& name) const {
      auto match = flags.find(name);
      if  (match == flags.end()) {
        return {};
      }
      return match->second;
    }

    std::size_t count_pos_args() const {
      return pos_args.size();
    }

    auto get_pos_args() const {
      return make_iterator_range(pos_args.cbegin(), pos_args.cend());
    }

  };

  using command_callback_t = std::function<int(const parse_result&)>;

  static constexpr const int opt = -3;
  static constexpr const int some = -2;
  static constexpr const int many = -1;

  class command {

    friend class program;

  protected:

    std::string _name;
    std::optional<std::string> _description;
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

    program& pos_arg(std::string name, int arity = 1) {
      _command.pos_arg(name, arity);
      return *this;
    }

    result<parse_result> parse_args(int argc, const char** argv) {

      int i = 0;
      std::vector<command*> command_stack { &_command };
      auto pos_arg_iter = command_stack.back()->_pos_args.begin();
      parse_result res; // Will hold all flags and positional argumments.
      int k = 0; // Counts the amount of positional arguments.

#define BOLT_PUSH_CMD(cmd) \
  if (pos_arg_iter != command_stack.back()->_pos_args.end()) { \
    return left(missing_pos_arg_error(*pos_arg_iter)); \
  } \
  command_stack.push_back(&cmd); \
  pos_arg_iter = cmd._pos_args.begin();

      for (; i < argc;) {

        std::string_view arg = argv[i++];

        if (arg[0] == '\0') {

          return left(invalid_argument_error(arg));

        } else if (arg[0] == '-') {

          //  TODO

        } else {

          // Match a command from the list of subcommands
          bool processed_arg = false;
          for (auto& cmd: command_stack.back()->_subcommands) {
            if (cmd._name == arg) {
              BOLT_PUSH_CMD(cmd);
              processed_arg = true;
            }
          }

          if (!processed_arg) {

            // Add a fallback command if no command matched
            for (;;) {
              bool changed = false;
              for (auto cmd: command_stack.back()->_subcommands) {
                if (cmd._is_fallback) {
                  BOLT_PUSH_CMD(cmd);
                  changed = true;
                  processed_arg = true;
                  break;
                }
              }
              if (!changed) {
                break;
              }
            }

            if (!processed_arg) {

              // Process any positional arguments
              if (pos_arg_iter == command_stack.back()->_pos_args.end()) {
                ZEN_PANIC("excess positional arguments provided");
              }
              res.add_pos_arg(std::string(arg));
              ++k;
              if (k == pos_arg_iter->_arity) {
                ++pos_arg_iter;
                k = 0;
              }

            }

          }

        }

      }

      if (!command_stack.empty() && command_stack.back()->_callback) {
        std::exit((*command_stack.back()->_callback)(res));
      }

      return right(res);
    }

  };

}

ZEN_NAMESPACE_END

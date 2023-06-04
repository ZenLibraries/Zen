
#include "zen/po.hpp"

#include <iostream> // For debugging only

ZEN_NAMESPACE_START

namespace po {

  result<match> program::parse_args(int argc, const char** argv) {
    std::vector<std::string_view> args;
    auto argv_end = argv+argc;
    ZEN_ASSERT(argv_end != argv);
    for (auto ptr = argv+1; ptr != argv_end; ++ptr) {
      args.push_back(*ptr);
    }
    return parse_args(args);
  }

  using parser_t = std::function<result<std::any>(const std::string&)>;

  std::unordered_map<std::type_index, parser_t> parsers {
    { std::type_index(typeid(std::string)), [](auto x) { return right(x); }  },
    { std::type_index(typeid(std::string)), [](auto x) { return right(x.empty() || x == "0" ? false : true); }  },
  };

  result<match> program::parse_args(std::vector<std::string_view> argv) {

    std::size_t i = 0;
    std::vector<command*> command_stack { &_command };
    auto pos_arg_iter = command_stack.back()->_pos_args.begin();
    auto res = new match;
    auto root_res = res;
    std::size_t pos_arg_count = 0; // Counts the amount of positional arguments.

#define BOLT_PUSH_CMD(cmd) \
  if (pos_arg_iter != command_stack.back()->_pos_args.end()) { \
    return left(missing_pos_arg_error(*pos_arg_iter)); \
  } \
  command_stack.push_back(&cmd); \
  pos_arg_iter = cmd._pos_args.begin(); \
  { \
    auto next_res = new match; \
    res->_subcommand = std::make_tuple(cmd._name, next_res); \
    res = next_res; \
  }

    for (; i < argv.size(); ) {

      auto arg = argv[i++];

      if (arg.size() == 0) {

        return left(invalid_argument_error(arg));

      } else if (arg[0] == '-') {

        std::size_t k = 1;
        if (arg.size() >= 2 && arg[1] == '-') {
          ++k;
        }
        std::string name;
        auto l = arg.find('=', k);
        if (l != arg.npos) {
          name = std::string(arg.substr(k, l));
        } else {
          name = std::string(arg.substr(k));
        }

        bool found = false;
        for (auto iter = command_stack.rbegin(); iter != command_stack.rend(); ++iter) {
          auto& cmd = **iter;
          auto match = cmd._flags.find(name);
          if (match == cmd._flags.end()) {
            std::cerr << "next" << std::endl;
            continue;
          }
          auto& flag = match->second;
          found = true;
          bool needs_value = flag.type != typeid(bool);
          if (!needs_value) {
            res->add_flag(name, true);
            break;
          }
          std::string value_str;
          if (l != arg.npos) {
            value_str = arg.substr(l);
          } else {
            if (i == argv.size()) {
              return left(flag_value_missing_error(name));
            }
            value_str = arg[i++];
          }
          auto parser_match = parsers.find(flag.type);
          if (parser_match == parsers.end()) {
            return left(unsupported_type_error(name));
          }
          auto value = parser_match->second(value_str);
          res->add_flag(name, value);
          break;
        }
        if (!found) {
          return left(unrecognised_flag_error(name));
        }

      } else {

        // Match a command from the list of subcommands
        for (auto& cmd: command_stack.back()->_subcommands) {
          if (cmd._name == arg) {
            BOLT_PUSH_CMD(cmd);
            goto next;
          }
        }

        // Add a fallback command if no command matched
        bool added_fallback = false;
        for (;;) {
          bool changed = false;
          for (auto cmd: command_stack.back()->_subcommands) {
            if (cmd._is_fallback) {
              BOLT_PUSH_CMD(cmd);
              changed = true;
              added_fallback = true;
              break;
            }
          }
          if (!changed) {
            break;
          }
        }

        // Process any positional arguments
        if (pos_arg_iter == command_stack.back()->_pos_args.end()) {
          if (added_fallback || command_stack.back()->_subcommands.empty()) {
            return left(excess_positional_arg_error(i, std::string(arg)));
          } else {
            return left(command_not_found_error(std::string(arg)));
          }
        }
        res->add_pos_arg(std::string(arg));
        ++pos_arg_count;
        if (pos_arg_count == pos_arg_iter->_arity) {
          ++pos_arg_iter;
          pos_arg_count = 0;
        }

      }

next:;
    }

    if (!command_stack.empty() && command_stack.back()->_callback) {
      std::exit((*command_stack.back()->_callback)(*root_res));
    }

    return right(*root_res);
  }

}

ZEN_NAMESPACE_END

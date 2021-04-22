
#include "zen/program_options.hpp"
#include <iostream>
#include <stdexcept>

namespace zen {

  flag_desc& program::find_flag(const std::string_view& name, const std::string_view& arg, std::vector<command_desc*>& command_stack) {
    for (auto iter = command_stack.rbegin(); iter != command_stack.rend(); iter++) {
      auto& command = **iter;
      auto flag_match = command.flags.find(name);
      if (flag_match != command.flags.end()) {
        return *flag_match->second;
      }
    }
    throw std::runtime_error("flag '" + std::string(arg) + "' not recognised");
  }

  template<typename T>
  static inline either<clone_ptr<parse_error>, T> parse_integral(std::string_view arg) {
    T result = 0;
    for (std::size_t i = 0; i < arg.size(); ++i) {
      auto ch = arg[i];
      if (!is_digit(ch)) {
        return left(make_cloned<invalid_integer_error>(std::string(arg)));
      }
      result += std::pow(10, i) * parse_decimal(ch);
    }
    return right(result);
  }

  parse_result<std::any> program::parse_value(flag_type type, const std::string_view& str) {
    switch (type) {
      case flag_type::ushort:
        return parse_integral<unsigned short>(str);
      case flag_type::uint:
        return parse_integral<unsigned int>(str);
      case flag_type::ulong:
        return parse_integral<unsigned long>(str);
      case flag_type::boolean:
        return right(str.size() > 0 && str != "0");
      case flag_type::string:
        return right(std::string(str));
    }
  }

  parse_result<parsed_args> program::parse_args(std::vector<std::string> args) {

    parsed_args result;
    std::size_t pos_index = 0;

    std::vector<command_desc*> command_stack { &command };

    for (std::size_t i = 0; i < args.size(); i++) {

      auto arg = args[i];

      if (arg.size() == 0) {
        throw std::runtime_error("unexpected empty argument");
      }

      if (arg[0] == '-') {

        auto k = 1;

        if (arg.size() == 1) {
          throw std::runtime_error("unexpected '-' as an argument");
        }
        if (arg[1] == '-') {
          k++;
        }
        if (arg.size() == 2) {
          std::vector<std::string> rest_args;
          for (std::size_t j = i; j < args.size(); j++) {
            rest_args.push_back(args[j]);
          }
          result["__"] = rest_args;
          break;
        }

        std::string_view name;
        std::string_view value_str;
        std::any value;

        auto l = arg.find_first_of('=', k);

        if (l == std::string::npos) {
          name = arg.substr(k);
          auto flag = find_flag(name, arg, command_stack);
          if (flag.type != flag_type::boolean) {
            i++;
            if (i == args.size()) {
              throw std::runtime_error("no argument provided for '" + arg + "'");
            }
          }
          value_str = args[i];
          auto res = parse_value(flag.type, args[i]);
          ZEN_TRY(res);
          value = *res;
        } else {
          name = arg.substr(k, l);
          auto flag = find_flag(name, arg, command_stack);
          auto res = parse_value(flag.type, arg.substr(l+1));
          ZEN_TRY(res);
          value = *res;
        }

        result.emplace(name, value);

        continue;

      } else {

        auto command = command_stack.back();

        if (!command->subcommands.empty()) {
          auto subcommand_match = command->subcommands.find(arg);
          if (subcommand_match == command->subcommands.end()) {
            return left(make_cloned<command_not_found_error>(arg));
          }
          command_stack.push_back(subcommand_match->second);
        } else {
          if (pos_index < command->pos_args.size()) {
            auto desc = command->pos_args[pos_index++];
            auto res = parse_value(desc->type, arg);
            ZEN_TRY(res);
            result.emplace(desc->name, *res);
          } else {
            return left(make_cloned<excess_arguments_error>(args, i));
          }
          result.emplace("_", arg);
        }

      }

    }
    return right(result);
  }

}


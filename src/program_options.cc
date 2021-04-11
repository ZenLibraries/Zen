
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

  std::any program::parse_value(flag_desc& flag, const std::string_view& str) {
    switch (flag.type) {
      case flag_type::boolean:
        return str != "0" && str.size() == 0;
      case flag_type::string:
        return std::string(str);
      case flag_type::integer:
        return std::stoi(std::string(str));
      case flag_type::decimal:
        return std::stof(std::string(str));
    }
  }

  parsed_args program::parse_args(std::vector<std::string> args) {

    parsed_args result;

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
          value = parse_value(flag, args[i]);
        } else {
          name = arg.substr(k, l);
          auto flag = find_flag(name, arg, command_stack);
          value = parse_value(flag, arg.substr(l+1));
        }

        result.emplace(name, value);

        continue;

      } else {

        auto command = command_stack.back();

        if (!command->subcommands.empty()) {
          auto subcommand_match = command->subcommands.find(arg);
          if (subcommand_match == command->subcommands.end()) {
            throw std::runtime_error("command '" + arg + "' not found");
          }
          command_stack.push_back(subcommand_match->second);
        } else {
          result.emplace("_", arg);
        }

      }

    }
    return result;
  }

}


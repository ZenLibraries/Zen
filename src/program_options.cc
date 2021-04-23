
#include "zen/program_options.hpp"
#include "zen/clone_ptr.hpp"
#include <iostream>
#include <stdexcept>

namespace zen {

  std::vector<std::string_view> subcommand_desc::get_path() const {
    std::vector<std::string_view> result;
    const command_desc* curr = this->parent;
    while (curr != nullptr) {
      result.push_back(curr->name);
      auto subcommand = dynamic_cast<const subcommand_desc*>(curr);
      if (subcommand == nullptr) {
        break;
      }
      curr = subcommand;
    }
    return result;
  }

  subcommand_desc* subcommands_desc::get_subcommand(const std::string_view& name) const {
    auto match = mapping.find(name);
    if (match == mapping.end()) {
      return nullptr;
    }
    return match->second;
  }

  subcommands_desc::~subcommands_desc() {
    for (auto& [name, subcommand]: mapping) {
      delete subcommand;
    }
  }

  pos_args_desc::~pos_args_desc() {
    for (auto arg: sequence) {
      delete arg;
    }
  }

  command_desc::~command_desc() {
    if (args != nullptr) {
      delete args;
    }
    for (auto& [name, flag]: flags) {
      delete flag;
    }
    // if (default_subcommand != nullptr) {
    //   delete default_subcommand;
    // }
  }

  flag_desc* program::find_flag(const std::string_view& name, const std::string_view& arg, std::vector<command_desc*>& command_stack) {
    for (auto iter = command_stack.rbegin(); iter != command_stack.rend(); iter++) {
      auto& command = **iter;
      auto flag_match = command.flags.find(name);
      if (flag_match != command.flags.end()) {
        return flag_match->second;
      }
    }
    return nullptr;
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

  static inline void write_repeat(std::ostream& out, std::string str, std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) {
      out << str;
    }
  }

  void program::print_help(const command_desc& command) const {

    auto pos_args = dynamic_cast<pos_args_desc*>(command.args);
    auto subcommands = dynamic_cast<subcommands_desc*>(command.args);

    std::cerr << desc.name;

    if (auto subcommand = dynamic_cast<const subcommand_desc*>(&command)) {
      for (auto name: subcommand->get_path()) {
        std::cerr << " " << name;
      }
    }

    if (pos_args != nullptr) {
      for (auto arg: pos_args->sequence) {
        if (arg->min_count == 1 && arg->max_count == 1) {
          std::cerr << " <" << arg->name << ">";
        } else if (arg->min_count == 0 && arg->max_count == 1) {
          std::cerr << " [" << arg->name << "]";
        } else if (arg->max_count == ZEN_AUTO_SIZE) {
          if (arg->min_count > 0) {
            std::cerr << " <" << arg->name << "..>";
          } else {
            std::cerr << " <" << arg->name << "..>";
          }
        } else {
          for (auto i = 0; i < arg->min_count; i++) {
            std::cerr << " <" << arg->name << ">";
          }
          for (auto i = arg->min_count; i < arg->max_count; i++) {
            std::cerr << " [" << arg->name << "]";
          }
        }
      }
    }

    std::cerr << "\n\n";

    if (command.description) {
      std::cerr << *command.description << "\n\n";
    }

    if (command.flags.size()) {
      std::cerr << "The following flags can be set:\n\n";
      std::size_t column1_width = 1;
      for (auto& [name, flags]: command.flags) {
        column1_width = std::max(column1_width, name.size());
      }
      for (auto& [name, flag]: command.flags) {
        std::cerr << "  --" << name;
        write_repeat(std::cerr, " ", column1_width - name.size());
        if (flag->description) {
          std::cerr << "  ";
          std::cerr << *flag->description;
        }
        std::cerr << "\n";
      }
    }

    if (subcommands != nullptr) {
      std::cerr << "\nThe following subcommands are available:\n\n";
      std::size_t column1_width = 1;
      for (auto& [name, subcommand]: subcommands->mapping) {
        column1_width = std::max(column1_width, subcommand->name.size());
      }
      for (auto& [name, subcommand]: subcommands->mapping) {
        std::cerr << "  " << name;
        write_repeat(std::cerr, " ", column1_width - name.size());
        if (subcommand->description) {
          std::cerr << "  ";
          std::cerr << *subcommand->description;
        }
        std::cerr << "\n";
      }
    }

  }

  void program::print_help() const {
    print_help(desc);
  }

  parse_result<void> program::validate_required(const command_desc& desc, parsed_args& result) {
    for (auto& [name, flag]: desc.flags) {
      if (!result.count(name) && flag->min_count > 0) {
        // TODO complete checks
        return left(make_cloned<flag_required_error>(*flag));
      }
    }
    if (auto pos_args = dynamic_cast<pos_args_desc*>(desc.args)) {
      for (auto pos_arg: pos_args->sequence) {
        if (pos_arg->min_count > 0) {
          // TODO
        }
      }
    }
    return right();
  }

  parse_result<parsed_args> program::parse_args_impl(
    std::vector<std::string> args,
    std::vector<command_desc*>& command_stack,
    std::size_t i,
    std::size_t& pos_index
  ) {

    parsed_args result;

    auto command = command_stack.back();

    if (command->default_subcommand != nullptr) {
      auto second_command_stack = command_stack;
      second_command_stack.push_back(command->default_subcommand);
      std::size_t second_pos_index = 0;
      std::size_t second_i = i;
      auto res = parse_args_impl(args, second_command_stack, second_i, second_pos_index);
      if (res.is_right()) {
        return right(*res);
      }
    }

    for (; i < args.size(); ++i) {

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
          ++k;
        }
        if (arg.size() == 2) {
          std::vector<std::string> rest_args;
          for (std::size_t j = i; j < args.size(); j++) {
            rest_args.push_back(args[j]);
          }
          result["__"] = rest_args;
          break;
        }

        flag_desc* flag;
        std::string_view name;
        std::string_view value_str;
        std::any value;

        auto l = arg.find_first_of('=', k);

        if (l == std::string::npos) {
          name = arg.substr(k);
          flag = find_flag(name, arg, command_stack);
          if (flag == nullptr) {
            return left(make_cloned<flag_not_found_error>(arg));
          }
          if (flag->type != flag_type::boolean) {
            ++i;
            if (i == args.size()) {
              throw std::runtime_error("no argument provided for '" + arg + "'");
            }
          }
          value_str = args[i];
          auto res = parse_value(flag->type, args[i]);
          ZEN_TRY(res);
          value = *res;
        } else {
          name = arg.substr(k, l);
          flag = find_flag(name, arg, command_stack);
          if (flag == nullptr) {
            return left(make_cloned<flag_not_found_error>(arg));
          }
          auto res = parse_value(flag->type, arg.substr(l+1));
          ZEN_TRY(res);
          value = *res;
        }

        if (flag->callback) {
          (*flag->callback)(*this, value);
        }
        result.emplace(name, value);

        continue;

      } else {

        auto& command = *command_stack.back();

        if (command.args == nullptr) {
          return left(make_cloned<excess_arguments_error>(args, i));
        }

        if (auto ptr = dynamic_cast<subcommands_desc*>(command.args)) {
          auto subcommand_match = ptr->mapping.find(arg);
          if (subcommand_match == ptr->mapping.end()) {
            return left(make_cloned<command_not_found_error>(arg));
          }
          ZEN_TRY2(validate_required(command, result));
          command_stack.push_back(subcommand_match->second);
          pos_index = 0;
          continue;
        }

        if (auto ptr = dynamic_cast<pos_args_desc*>(command.args)) {
          auto desc = ptr->sequence[pos_index];
          auto res = parse_value(desc->type, arg);
          ZEN_TRY(res);
          result.emplace(desc->name, *res);
          ++pos_index;
          continue;
        }

        ZEN_UNREACHABLE

      }

    }

    command = command_stack.back();

    if (command->has_subcommands() && result.size() == 0) {
      print_help(*command);
      std::exit(1);
    }

    if (command_stack.size() > 1) {
      auto& command = *static_cast<subcommand_desc*>(command_stack.back());
      ZEN_TRY2(validate_required(command, result));
      if (command.callback) {
        command.callback(result);
      }
    }

    return right(result);
  }

  parse_result<parsed_args> program::parse_args(std::vector<std::string> args) {
    std::vector<command_desc*> command_stack { &command };
    std::size_t pos_index = 0;
    std::size_t i = 0;
    return parse_args_impl(args, command_stack, i, pos_index);
  }

  parse_result<parsed_args> program::parse_args(int argc, const char* argv[]) {
    std::vector<std::string> args { argv+1, argv + argc };
    return parse_args(args);
  }

}


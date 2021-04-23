
#include "zen/program_options.hpp"
#include "zen/clone_ptr.hpp"
#include "zen/config.hpp"
#include <iostream>
#include <ostream>
#include <stdexcept>

namespace zen {

  std::vector<std::string_view> subcommand_desc::get_path() const {
    std::vector<std::string_view> result;
    const command_desc* curr = this;
    while (curr != nullptr) {
      auto subcommand = dynamic_cast<const subcommand_desc*>(curr);
      if (subcommand == nullptr) {
        break;
      }
      result.push_back(curr->name);
      curr = subcommand->parent;
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
    for (auto iter = command_stack.rbegin(); iter != command_stack.rend(); ++iter) {
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

  template<typename T>
  void append_to_vector(std::any target, std::any value) {
    auto& vector = std::any_cast<std::vector<T>&>(target);
    vector.push_back(std::any_cast<T>(value));
  }

  void append_to_vector(std::any& target, std::any value) {
    if (value.type() == typeid(unsigned long)) {
      append_to_vector<unsigned long>(target, value);
    }
    if (value.type() == typeid(unsigned short)) {
      append_to_vector<unsigned short>(target, value);
    }
    if (value.type() == typeid(std::string)) {
      append_to_vector<std::string>(target, value);
    }
    ZEN_UNREACHABLE
  }

  std::any create_vector(flag_type type) {
    switch (type) {
      case flag_type::ulong:
        return std::vector<unsigned long>();
      case flag_type::uint:
        return std::vector<unsigned int>();
      case flag_type::ushort:
        return std::vector<unsigned short>();
      case flag_type::boolean:
        return std::vector<bool>();
      case flag_type::string:
        return std::vector<std::string>();
    }
  }

  template<typename ContainerT, typename KeyT, typename ValueT>
  void replace(ContainerT& container, const KeyT& key, ValueT&& value) {
    auto match = container.find(key);
    if (match == container.end()) {
      container.emplace(key, value);
    } else {
      match->second = std::move(value);
    }
  }

  static inline bool starts_with(const std::string_view str, const std::string_view prefix, std::size_t start_offset) {
    for (auto i = start_offset; i < str.size(); ++i) {
      if (i == prefix.size()) {
        break;
      }
      if (prefix[i - start_offset] != str[i]) {
        return false;
      }
    }
    return true;
  }

  parse_result<void> program::parse_args_impl(
    const std::vector<std::string>& args,
    parsed_args& result,
    std::vector<command_desc*>& command_stack,
    std::size_t i
  ) {

    std::size_t pos_index = 0;

    auto command = command_stack.back();

    for (auto& [name, flag]: command->flags) {
      if (flag->default_value.has_value()) {
        result.emplace(name, flag->default_value);
      }
    }

    if (command->default_subcommand != nullptr) {
      auto next_command_stack = command_stack;
      next_command_stack.push_back(command->default_subcommand);
      auto next_result = result;
      auto res = parse_args_impl(args, next_result, next_command_stack, i);
      if (res.is_right()) {
        command_stack = next_command_stack;
        result = next_result;
        return right();
      }
    }

    for (; i < args.size(); ++i) {

      auto arg = args[i];
      bool negate = false;

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
        if (starts_with(arg, "no-", 2)) {
          negate = true;
          k = 5;
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
          if (flag->type == flag_type::boolean) {
            value = true;
          } else {
            ++i;
            if (i == args.size()) {
              throw std::runtime_error("no argument provided for '" + arg + "'");
            }
            value_str = args[i];
            auto res = parse_value(flag->type, args[i]);
            ZEN_TRY(res);
            value = *res;
          }
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
        if (flag->max_count > 1) {
          auto match = result.find(std::string(name));
          if (match != result.end()) {
            append_to_vector(match->second, value);
          } else {
            auto vector = create_vector(flag->type);
            append_to_vector(vector, value);
            replace(result, std::string(name), vector);
          }
        } else {
          replace(result, std::string(name), value);
        }

        continue;

      } else {

        if (command->args == nullptr) {
          return left(make_cloned<excess_arguments_error>(args, i));
        }

        if (auto ptr = dynamic_cast<subcommands_desc*>(command->args)) {
          auto subcommand_match = ptr->mapping.find(arg);
          if (subcommand_match == ptr->mapping.end()) {
            return left(make_cloned<command_not_found_error>(arg));
          }
          ZEN_TRY2(validate_required(*command, result));
          command_stack.push_back(subcommand_match->second);
          command = subcommand_match->second;
          ++i;
          return parse_args_impl(args, result, command_stack, i);
        }

        if (auto ptr = dynamic_cast<pos_args_desc*>(command->args)) {
          if (pos_index == ptr->sequence.size()) {
            return left(make_cloned<excess_arguments_error>(args, i));
          }
          auto desc = ptr->sequence[pos_index];
          auto res = parse_value(desc->type, arg);
          ZEN_TRY(res);
          replace(result, desc->name, *res);
          ++pos_index;
          continue;
        }

        ZEN_UNREACHABLE

      }

    }

    if (command->has_subcommands() && result.size() == 0) {
      print_help(*command);
      std::exit(1);
    }

    for (auto command: command_stack) {
      ZEN_TRY2(validate_required(*command, result));
    }

    return right();
  }

  parse_result<parsed_args> program::parse_args(std::vector<std::string> args) {
    parsed_args result;
    std::vector<command_desc*> command_stack { &command };
    std::size_t i = 0;
    auto res = parse_args_impl(args, result, command_stack, i);
    ZEN_TRY(res);
    int status;
    for (auto command: command_stack) {
      if (auto subcommand = dynamic_cast<subcommand_desc*>(command)) {
        if (subcommand->callback) {
          status = (*subcommand->callback)(result);
          if (status != 0) {
            break;
          }
        }
      }
    }
    // if (status != 0) {
    //   return left(make_cloned<nonzero_exit_code_error>(status));
    // }
    return right(result);
  }

  parse_result<parsed_args> program::parse_args(int argc, const char* argv[]) {
    std::vector<std::string> args { argv+1, argv + argc };
    return parse_args(args);
  }

  void print_error(const parse_error& error, std::ostream& out) {
    error.print(out);
  }

  void invalid_integer_error::print(std::ostream& out) const {
    out << "error: could not parse '" << str << "' as a valid integer\n";
  }

  void flag_required_error::print(std::ostream &out) const {
    out << "error: the flag '--" << desc.name << "' must be set\n";
  }

  void flag_not_found_error::print(std::ostream &out) const {
    out << "error: the argument '" << arg << "' was not recognised\n";
  }

  void command_not_found_error::print(std::ostream& out) const {
    out << "error: the command '" << name << "' was not found\n";
  }

  void excess_arguments_error::print(std::ostream &out) const {
    out << "error: the argument '" << args[i] << "' was not recognised\n";
  }

  void missing_argument_error::print(std::ostream& out) const {
    out << "error: missing argument for flag '" << name << "'\n";
  }

}


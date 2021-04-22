#ifndef ZEN_PROGRAM_OPTIONS_HPP
#define ZEN_PROGRAM_OPTIONS_HPP

#include <cmath>
#include <optional>
#include <any>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "zen/char.hpp"
#include "zen/assoc_list.hpp"
#include "zen/clone_ptr.hpp"
#include "zen/either.hpp"

namespace zen {

  class command_builder;
  class subcommand_builder;
  struct flag_desc;
  struct command_desc;
  struct subcommand_desc;
  class program;

  using parsed_args = std::unordered_map<std::string, std::any>;

  enum class flag_type {
    boolean,
    string,
    ulong,
    uint,
    ushort,
  };

  struct arg_desc {
    flag_type type;
    std::string name;
    std::optional<std::string> description;
    int min_count = 1;
    int max_count = 1;
  };

  struct flag_desc {
    flag_type type;
    std::string name;
    std::optional<std::string> description;
    int min_count = 1;
    int max_count = 1;
    std::any default_value;
  };

  class arg_builder {

    friend class program;

    arg_desc& desc;

  public:

    inline arg_builder(arg_desc& desc):
      desc(desc) {}

    inline arg_builder& set_description(std::string new_description) {
      desc.description = new_description;
      return *this;
    }

    inline arg_builder& set_max_count(std::size_t new_max_count) {
      desc.max_count = new_max_count;
      return *this;
    }

  };

  class flag_builder {

    friend class program;

    flag_desc& desc;

  public:

    inline flag_builder(flag_desc& desc):
      desc(desc) {}

    inline flag_builder& set_description(std::string new_description) {
      desc.description = new_description;
      return *this;
    }

    inline flag_builder& set_default_value(std::any new_value) {
      desc.default_value = new_value;
      return *this;
    }

  };

  struct command_desc {
    std::string name;
    std::optional<std::string> description;
    assoc_list<std::string, subcommand_desc*> subcommands;
    std::vector<arg_desc*> pos_args;
    assoc_list<std::string, flag_desc*> flags;
  };

  struct subcommand_desc : public command_desc {

  };

  class command_builder {

    friend class program;

    command_desc& desc;

    subcommand_desc* get_subcommand(const std::string_view& name) {
      auto match = desc.subcommands.find(name);
      if (match == desc.subcommands.end()) {
        return nullptr;
      }
      return match->second;
    }

  public:

    inline command_builder(command_desc& command):
      desc(command) {}

    inline command_builder& set_description(std::string new_description) {
      desc.description = new_description;
      return *this;
    }

    inline arg_builder add_string_arg(std::string name) {
      auto arg = new arg_desc;
      arg->type = flag_type::string;
      arg->name = name;
      desc.pos_args.push_back(arg);
      return arg_builder(*arg);
    }

    inline flag_builder add_bool_flag(std::string name) {
      auto flag = new flag_desc;
      flag->type = flag_type::boolean;
      flag->name = name;
      desc.flags.push_back(name, flag);
      return flag_builder(*flag);
    }

    inline flag_builder add_ulong_flag(std::string name) {
      auto flag = new flag_desc;
      flag->type = flag_type::ulong;
      desc.flags.push_back(name, flag);
      return flag_builder(*flag);
    }

    inline flag_builder add_short_flag(std::string name) {
      auto flag = new flag_desc;
      flag->type = flag_type::ushort;
      desc.flags.push_back(name, flag);
      return flag_builder(*flag);
    }

    inline flag_builder add_bool_flag(std::string name, std::string description) {
      auto flag = new flag_desc;
      flag->type = flag_type::boolean;
      flag->name = name;
      flag->description = description;
      desc.flags.push_back(name, flag);
      return flag_builder(*flag);
    }

    bool has_subcommands() const {
      return desc.subcommands.size() > 0;
    }

    subcommand_builder add_subcommand(std::string name);

    ~command_builder();

  };

  class subcommand_builder : public command_builder {

  };

  using arg_list = std::vector<std::string>;

  class parse_error {
  public:
    virtual parse_error* clone() = 0;
    virtual ~parse_error() = default;
  };

  class invalid_integer_error : public parse_error {
  public:

    std::string str;

    invalid_integer_error(std::string str):
      str(str) {}

    parse_error* clone() {
      return new invalid_integer_error(str);
    }

  };

  class command_not_found_error : public parse_error {
  public:

    std::string name;

    command_not_found_error(std::string name):
      name(name) {}

    command_not_found_error* clone() {
      return new command_not_found_error(name);
    }

  };

  class excess_arguments_error : public parse_error {
  public:

    arg_list args;
    std::size_t i;

    excess_arguments_error(arg_list args, std::size_t i):
      args(args), i(i) {}

    excess_arguments_error* clone() {
      return new excess_arguments_error(args, i);
    }

  };

  class parse_context {
  public:
    arg_list args;
    arg_list::iterator curr_arg;
  };

  template<typename T, typename Enabler = void>
  struct value_parser;

  template<typename T>
  using parse_result = either<clone_ptr<parse_error>, T>;

  class program : public command_builder {

    parse_result<std::any> parse_value(flag_type type, const std::string_view& str);

    flag_desc& find_flag(
      const std::string_view& name,
      const std::string_view& arg,
      std::vector<command_desc*>& command_stack
    );

    command_desc command;

  public:

    inline program(std::string new_name):
        command_builder(command) {
      command.name = new_name;
    }

    inline program(std::string name, std::string description):
        command_builder(command) {
      command.name = name;
      command.description = description;
    }

    parse_result<parsed_args> parse_args(std::vector<std::string> args);

  };

  inline subcommand_builder command_builder::add_subcommand(std::string name) {
    auto subcommand = new subcommand_desc;
    subcommand->name = name;
    desc.subcommands.push_back(name, subcommand);
    return subcommand_builder { *subcommand };
  }

  inline command_builder::~command_builder() {
    for (auto& [name, flag]: desc.flags) {
      delete flag;
    }
    for (auto& [name, command]: desc.subcommands) {
      delete command;
    }
    for (auto arg: desc.pos_args) {
      delete arg;
    }
  }

}

#endif // of #ifndef ZEN_PROGRAM_OPTIONS_HPP

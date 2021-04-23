#ifndef ZEN_PROGRAM_OPTIONS_HPP
#define ZEN_PROGRAM_OPTIONS_HPP

#include <cmath>
#include <optional>
#include <any>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <functional>

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

  struct desc_base {
    virtual ~desc_base() = default;
  };

  struct arg_desc : public desc_base {
    flag_type type;
    std::string name;
    std::optional<std::string> description;
    int min_count = 1;
    int max_count = 1;
  };

  struct flag_desc : public desc_base {
    flag_type type;
    std::string name;
    std::optional<std::string> description;
    int min_count = 0;
    int max_count = 1;
    std::any default_value;
  };

  struct subcommands_desc : public desc_base {
    assoc_list<std::string, subcommand_desc*> mapping;
    ~subcommands_desc();
  };

  struct command_desc : public desc_base {

    std::string name;
    std::optional<std::string> description;
    std::vector<desc_base*> pos_args;
    subcommand_desc* default_subcommand = nullptr;
    assoc_list<std::string, flag_desc*> flags;

    bool has_subcommands() const {
      for (auto arg: pos_args) {
        if (dynamic_cast<subcommands_desc*>(arg)) {
          return true;
        }
      }
      return false;
    }

    ~command_desc();
  };

  struct program_desc : public command_desc {
    std::string version;
  };

  using subcommand_callback = std::function<int(parsed_args)>;

  struct subcommand_desc : public command_desc {
    subcommand_callback callback;
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

  template<typename BaseT, typename DescT>
  class command_builder_base {
  protected:

    friend class program;
    friend class subcommand_builder;

    DescT& desc;
    subcommands_desc* subcommands = nullptr;

    subcommand_desc* get_subcommand(const std::string_view& name) {
      if (subcommands == nullptr) {
        return nullptr;
      }
      auto match = desc.subcommands.find(name);
      if (match == desc.subcommands.end()) {
        return nullptr;
      }
      return match->second;
    }

  public:

    inline command_builder_base(DescT& command):
      desc(command) {}

    inline BaseT& set_description(std::string new_description) {
      desc.description = new_description;
      return *static_cast<BaseT*>(this);
    }

    inline DescT& get_desc() const {
      return desc;
    }

    BaseT& set_default_subcommand(subcommand_builder& subcommand);

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
      if (desc.subcommands == nullptr) {
        return false;
      }
      return desc.subcommands.size() > 0;
    }

    subcommand_builder add_subcommand(std::string name);

  };

  class subcommand_builder : public command_builder_base<subcommand_builder, subcommand_desc> {
  public:

    subcommand_builder& set_callback(subcommand_callback callback) {
      desc.callback = callback;
      return *this;
    }

  };

  template<typename BaseT, typename DescT>
  BaseT& command_builder_base<BaseT, DescT>::set_default_subcommand(subcommand_builder& builder) {
    desc.default_subcommand = &builder.get_desc();
    return *static_cast<BaseT*>(this);
  }

  template<typename BaseT, typename DescT>
  inline subcommand_builder command_builder_base<BaseT, DescT>::add_subcommand(std::string name) {
    if (subcommands == nullptr) {
      subcommands = new subcommands_desc;
      desc.pos_args.push_back(subcommands);
    }
    auto subcommand = new subcommand_desc;
    subcommand->name = name;
    subcommands->mapping.push_back(name, subcommand);
    return subcommand_builder { *subcommand };
  }

  using arg_list = std::vector<std::string>;

  class parse_error {
  public:
    virtual parse_error* clone() const = 0;
    virtual ~parse_error() = default;
  };

  class invalid_integer_error : public parse_error {
  public:

    std::string str;

    invalid_integer_error(std::string str):
      str(str) {}

    parse_error* clone() const override {
      return new invalid_integer_error(str);
    }

  };

  class flag_required_error : public parse_error {
  public:

    flag_desc& desc;

    flag_required_error(flag_desc& desc):
      desc(desc) {}

    flag_required_error* clone() const override {
      return new flag_required_error(desc);
    }

  };

  class flag_not_found_error : public parse_error {
  public:

    std::string arg;

    flag_not_found_error(std::string arg):
      arg(arg) {}

    flag_not_found_error* clone() const override {
      return new flag_not_found_error(arg);
    }

  };

  class command_not_found_error : public parse_error {
  public:

    std::string name;

    command_not_found_error(std::string name):
      name(name) {}

    command_not_found_error* clone() const override {
      return new command_not_found_error(name);
    }

  };

  class excess_arguments_error : public parse_error {
  public:

    arg_list args;
    std::size_t i;

    excess_arguments_error(arg_list args, std::size_t i):
      args(args), i(i) {}

    excess_arguments_error* clone() const override {
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

  class program : public command_builder_base<program, program_desc> {

    bool enable_help = true;

    parse_result<std::any> parse_value(flag_type type, const std::string_view& str);

    flag_desc* find_flag(
      const std::string_view& name,
      const std::string_view& arg,
      std::vector<command_desc*>& command_stack
    );

    program_desc command;

    parse_result<void> validate_required(const command_desc& desc, parsed_args& result);

    parse_result<parsed_args> parse_args_impl(
      std::vector<std::string> args,
      std::vector<command_desc*>& command_stack,
      std::size_t i,
      std::size_t& pos_index
    );

  public:

    inline program(std::string new_name):
      command_builder_base(command) {
        command.name = new_name;
      }

    inline program(std::string name, std::string description):
      command_builder_base(command) {
        command.name = name;
        command.description = description;
      }

    void print_help() const;

    parse_result<parsed_args> parse_args(std::vector<std::string> args);
    parse_result<parsed_args> parse_args(int argc, const char* argv[]);

  };

}

#endif // of #ifndef ZEN_PROGRAM_OPTIONS_HPP

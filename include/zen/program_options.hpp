#ifndef ZEN_PROGRAM_OPTIONS_HPP
#define ZEN_PROGRAM_OPTIONS_HPP

#include <optional>
#include <any>
#include <memory>
#include <unordered_map>

#include "zen/assoc_list.hpp"

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
    integer,
    decimal,
  };

  struct flag_desc {
    flag_type type;
    std::string name;
    std::optional<std::string> description;
    int min_count = 1;
    int max_count = 1;
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

  };

  struct command_desc {
    std::string name;
    std::optional<std::string> description;
    assoc_list<std::string, subcommand_desc*> subcommands;
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

    inline flag_builder add_bool_flag(std::string name) {
      auto flag = new flag_desc;
      flag->type = flag_type::boolean;
      flag->name = name;
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

  class program : public command_builder {

    std::any parse_value(flag_desc& flag, const std::string_view& str);

    flag_desc& find_flag(const std::string_view& name, const std::string_view& arg, std::vector<command_desc*>& command_stack);

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

    parsed_args parse_args(std::vector<std::string> args);

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
  }

}

#endif // of #ifndef ZEN_PROGRAM_OPTIONS_HPP

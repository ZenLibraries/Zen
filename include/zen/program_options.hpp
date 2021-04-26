#ifndef ZEN_PROGRAM_OPTIONS_HPP
#define ZEN_PROGRAM_OPTIONS_HPP

#include <cmath>
#include <optional>
#include <any>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <functional>
#include <typeindex>

#include "zen/char.hpp"
#include "zen/assoc_list.hpp"
#include "zen/clone_ptr.hpp"
#include "zen/either.hpp"

ZEN_NAMESPACE_START

class parse_error;
class command_builder;
class subcommand_builder;
struct flag_desc;
struct command_desc;
struct subcommand_desc;
class program;

void print_error(const parse_error& error, std::ostream& out = std::cerr);

using arg_list = std::vector<std::string>;

class parse_result {

  arg_list rest_args;

  std::unordered_map<std::string, std::any> mapping;

public:

  template<typename T>
  const std::optional<T> get_value(const std::string& name) const {
    auto match = mapping.find(name);
    if (match == mapping.end()) {
      return {};
    }
    return std::any_cast<T>(match->second);
  }

  template<typename T>
  const T get_value(const std::string& name, T default_value) const {
    auto match = mapping.find(name);
    if (match == mapping.end()) {
      return default_value;
    }
    return std::any_cast<T>(match->second);
  }

  void set_value(std::string name, std::any value) {
    auto match = mapping.find(name);
    if (match != mapping.end()) {
      match->second = value;
    } else {
      mapping.emplace(name, value);
    }
  }

  void append_value(std::string name, std::any value);

  std::size_t count_values() const {
    return mapping.size();
  }

  bool has_value(const std::string& name) const {
    return mapping.count(name);
  }

  void set_rest_args(std::vector<std::string> args) {
    rest_args = args;
  }

  const arg_list& get_rest_args() const {
    return rest_args;
  }

};

struct desc_base {
  virtual ~desc_base() = default;
};

struct pos_arg_desc : public desc_base {

  std::type_index type;
  std::string name;
  std::optional<std::string> description;
  unsigned int min_count;
  unsigned int max_count;

  pos_arg_desc(
    std::type_index type,
    std::string name,
    std::optional<std::string> description = {},
    unsigned int min_count = 1,
    unsigned int max_count = 1
  ): type(type),
     name(name),
     description(description),
     min_count(min_count),
     max_count(max_count) {}

};

using flag_callback = std::function<void(program&, std::any)>;

struct flag_desc : public desc_base {

  std::type_index type;
  std::string name;
  std::optional<std::string> description;
  std::optional<flag_callback> callback;
  unsigned int min_count = 0;
  unsigned int max_count = 1;
  bool fallthrough = false;
  std::any default_value;

  flag_desc(
    std::type_index type,
    std::string name,
    std::optional<std::string> description = {},
    std::optional<flag_callback> callback = {},
    unsigned int min_count = 0,
    unsigned int max_count = 1,
    bool fallthrough = false,
    std::any default_value = {}
  ): type(type),
     name(name),
     description(description),
     callback(callback),
     min_count(min_count),
     max_count(min_count),
     fallthrough(fallthrough),
     default_value(default_value) {

     }

};

struct args_desc_base : public desc_base {
};

struct subcommands_desc : public args_desc_base {

  assoc_list<std::string, subcommand_desc*> mapping;

  subcommand_desc* get_subcommand(const std::string_view& name) const;

  ~subcommands_desc();
};

struct pos_args_desc : public args_desc_base {

  std::vector<pos_arg_desc*> sequence;

  void append_pos_arg(pos_arg_desc* desc) {
    sequence.push_back(desc);
  }

  ~pos_args_desc();
};

struct command_desc : public desc_base {

  std::size_t pos_arg_count = 0;
  std::size_t subcommand_count = 0;

  std::string name;
  std::optional<std::string> description;
  args_desc_base* args = nullptr;
  subcommand_desc* default_subcommand = nullptr;
  assoc_list<std::string, flag_desc*> flags;

  bool has_subcommands() const {
    return subcommand_count > 0;
  }

  ~command_desc();
};

struct program_desc : public command_desc {
  std::string version;
};

using subcommand_callback = std::function<int(const parse_result&)>;

struct subcommand_desc : public command_desc {

  command_desc* parent = nullptr;
  std::optional<subcommand_callback> callback;

  std::vector<std::string_view> get_path() const;

};

class arg_builder {

  friend class program;

  pos_arg_desc& desc;

public:

  inline arg_builder(pos_arg_desc& desc):
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

template<typename T, typename Enabler = void>
struct construct;

template<typename T>
struct construct<T, std::enable_if_t<std::is_fundamental_v<T>>> {
  template<typename R>
  static T apply(R value) {
    return static_cast<T>(value);
  }
};

template<typename T>
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

  inline flag_builder& set_default_value(T new_value) {
    desc.default_value = new_value;
    return *this;
  }

};

inline std::size_t count_starting_chars(std::string_view str, char ch) {
  for (auto i = 0; i < str.size(); ++i) {
    if (str[i] != ch) {
      return i;
    }
  }
  return str.size();
}

template<typename BaseT, typename DescT>
class command_builder_base {
protected:

  friend class program;
  friend class subcommand_builder;

  DescT& desc;

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

  template<typename T>
  inline arg_builder add_pos_arg(std::string name) {
    auto arg = new pos_arg_desc { typeid(T), name };
    pos_args_desc* pos_args;
    if (desc.args == nullptr) {
      desc.args = pos_args = new pos_args_desc;
    } else {
      pos_args = dynamic_cast<pos_args_desc*>(desc.args);
      if (pos_args == nullptr) {
        ZEN_PANIC("Trying to add a positional argument to a command which already contains subcommands");
      }
    }
    pos_args->append_pos_arg(arg);
    return arg_builder(*arg);
  }

  template<typename T>
  inline auto add_flag(std::string name, std::optional<std::string> description = {}) {
    std::size_t k = count_starting_chars(name, '-');
    name = name.substr(k);
    auto flag = new flag_desc { typeid(T), name, description };
    desc.flags.push_back(name, flag);
    return flag_builder<T>(*flag);
  }

  inline BaseT& add_help_flag();

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
  subcommands_desc* subcommands;
  if (desc.args == nullptr) {
    desc.args = subcommands = new subcommands_desc;
  } else {
    subcommands = dynamic_cast<subcommands_desc*>(desc.args);
    if (subcommands == nullptr) {
      ZEN_PANIC("Trying to add a subcommand to a command that already contains positional arguments.");
    }
  }
  auto subcommand = new subcommand_desc;
  subcommand->parent = &desc;
  subcommand->name = name;
  subcommands->mapping.push_back(name, subcommand);
  return subcommand_builder { *subcommand };
}

class parse_error {
  friend void print_error(const parse_error& error, std::ostream& out);
public:
  virtual void print(std::ostream& out) const = 0;
  virtual parse_error* clone() const = 0;
  virtual ~parse_error() = default;
};

class invalid_integer_error : public parse_error {
public:

  std::string str;

  invalid_integer_error(std::string str):
    str(str) {}

  void print(std::ostream& out) const override;

  parse_error* clone() const override {
    return new invalid_integer_error(str);
  }

};

class invalid_bool_error : public parse_error {
public:

  std::string str;

  invalid_bool_error(std::string str):
    str(str) {};

  void print(std::ostream& out) const override;

  invalid_bool_error* clone() const override {
    return new invalid_bool_error(str);
  }

};

class flag_required_error : public parse_error {
public:

  flag_desc& desc;

  flag_required_error(flag_desc& desc):
    desc(desc) {}

  void print(std::ostream& out) const override;

  flag_required_error* clone() const override {
    return new flag_required_error(desc);
  }

};

class flag_not_found_error : public parse_error {
public:

  std::string arg;

  flag_not_found_error(std::string arg):
    arg(arg) {}

  void print(std::ostream& out) const override;

  flag_not_found_error* clone() const override {
    return new flag_not_found_error(arg);
  }

};

class command_not_found_error : public parse_error {
public:

  std::string name;

  command_not_found_error(std::string name):
    name(name) {}

  void print(std::ostream& out) const override;

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

  void print(std::ostream& out) const override;

  excess_arguments_error* clone() const override {
    return new excess_arguments_error(args, i);
  }

};

class missing_argument_error : public parse_error {
public:

  arg_list args;
  std::string name;

  missing_argument_error(arg_list args, std::string name):
    args(args), name(name) {}

  void print(std::ostream& out) const override;

  missing_argument_error* clone() const override {
    return new missing_argument_error(args, name);
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
using result = either<clone_ptr<parse_error>, T>;

class program : public command_builder_base<program, program_desc> {

  result<std::any> parse_value(const std::type_index& type, const std::string_view& str);

  flag_desc* find_flag(
    const std::string_view& name,
    const std::string_view& arg,
    std::vector<command_desc*>& command_stack
  );

  program_desc command;

  result<void> validate_required(const command_desc& desc, parse_result& result);

  result<void> parse_args_impl(
    const std::vector<std::string>& args,
    parse_result& result,
    std::vector<command_desc*>& command_stack,
    std::size_t i
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
  void print_help(const command_desc& desc) const;

  result<parse_result> parse_args(std::vector<std::string> args);
  result<parse_result> parse_args(int argc, const char* argv[]);

};

template<typename BaseT, typename DescT>
inline BaseT& command_builder_base<BaseT, DescT>::add_help_flag() {
  auto flag = new flag_desc { typeid(bool), "help" };
  flag->description = "Print more infomation about this command";
  flag->callback = [&] (program& prog, std::any value) {
    prog.print_help(this->desc);
    std::exit(1);
  };
  desc.flags.push_back("help", flag);
  return *static_cast<BaseT*>(this);
}

ZEN_NAMESPACE_END

#endif // of #ifndef ZEN_PROGRAM_OPTIONS_HPP

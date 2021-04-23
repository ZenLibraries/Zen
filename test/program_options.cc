
#include "gtest/gtest.h"

#include "zen/program_options.hpp"

using namespace zen;

TEST(ProgramOptionsTest, CanParseCommandWithBoolFlagAndPosArgs) {
  program prog("foo", "A random test command");
  auto command_one = prog.add_subcommand("command-one");
  command_one.add_bool_flag("bar", "Enable the 'bar' feature");
  command_one.add_string_arg("one");
  command_one.add_string_arg("two");
  command_one.add_string_arg("three");
  auto parsed_1 = prog.parse_args({ "command-one", "--bar", "1", "2", "3" }).unwrap();
  ASSERT_TRUE(parsed_1.find("bar") != parsed_1.end());
  ASSERT_TRUE(std::any_cast<bool>(parsed_1["bar"]));
  ASSERT_EQ(std::any_cast<std::string>(parsed_1["one"]), "1");
  ASSERT_EQ(std::any_cast<std::string>(parsed_1["two"]), "2");
  ASSERT_EQ(std::any_cast<std::string>(parsed_1["three"]), "3");
}

TEST(ProgramOptionsTest, ErrorsWhenCommandNotFound) {
  program prog("foo", "A random test command");
  auto command_one = prog.add_subcommand("command-one");
  command_one.add_bool_flag("bar", "Enable the 'bar' feature");
  command_one.add_string_arg("one");
  command_one.add_string_arg("two");
  command_one.add_string_arg("three");
  auto parsed_1 = prog.parse_args({ "no-command" }).unwrap_left();
  ASSERT_NE(dynamic_cast<command_not_found_error*>(parsed_1.get()), nullptr);
}

TEST(ProgramOptionsTest, UsesDefaultCommand) {
  program prog("foo", "A random test command");
  auto command_one = prog.add_subcommand("command-one");
  command_one.add_bool_flag("foo");
  command_one.add_bool_flag("foob");
  auto command_two = prog.add_subcommand("command-two");
  prog.set_default_subcommand(command_one);
  command_two.add_bool_flag("bar");
  command_two.add_bool_flag("bax");
  auto parsed_1 = prog.parse_args({ "--foo" }).unwrap();
  auto parsed_2 = prog.parse_args({ "--bar", "--bax" }).unwrap_left();
}


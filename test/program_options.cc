
#include "gtest/gtest.h"

#include "zen/program_options.hpp"

using namespace zen;

TEST(ProgramOptionsTest, CanParseCommandWithBoolFlagAndPosArgs) {
  program prog("foo", "A random test command");
  auto command_one = prog.add_subcommand("command-one");
  command_one.add_flag<bool>("bar", "Enable the 'bar' feature");
  command_one.add_pos_arg<std::string>("one");
  command_one.add_pos_arg<std::string>("two");
  command_one.add_pos_arg<std::string>("three");
  auto parsed_1 = prog.parse_args({ "command-one", "--bar", "1", "2", "3" }).unwrap();
  ASSERT_TRUE(parsed_1.has_value("bar"));
  ASSERT_EQ(*parsed_1.get_value<bool>("bar"), true);
  ASSERT_EQ(parsed_1.get_value<std::string>("one"), "1");
  ASSERT_EQ(parsed_1.get_value<std::string>("two"), "2");
  ASSERT_EQ(parsed_1.get_value<std::string>("three"), "3");
}

TEST(ProgramOptionsTest, ErrorsWhenCommandNotFound) {
  program prog("foo", "A random test command");
  auto command_one = prog.add_subcommand("command-one");
  command_one.add_flag<bool>("bar", "Enable the 'bar' feature");
  command_one.add_pos_arg<std::string>("one");
  command_one.add_pos_arg<std::string>("two");
  command_one.add_pos_arg<std::string>("three");
  auto parsed_1 = prog.parse_args({ "no-command" }).unwrap_left();
  ASSERT_NE(dynamic_cast<command_not_found_error*>(parsed_1.get()), nullptr);
}

TEST(ProgramOptionsTest, UsesDefaultCommand) {
  program prog("foo", "A random test command");
  auto command_one = prog.add_subcommand("command-one");
  command_one.add_flag<bool>("foo");
  command_one.add_flag<bool>("foob");
  auto command_two = prog.add_subcommand("command-two");
  prog.set_default_subcommand(command_one);
  command_two.add_flag<std::string>("bar");
  command_two.add_flag<std::string>("bax");
  auto parsed_1 = prog.parse_args({ "--foo" }).unwrap();
  auto parsed_2 = prog.parse_args({ "--bar", "--bax" }).unwrap_left();
}



#include "gtest/gtest.h"

#include "zen/program_options.hpp"

using namespace zen;

TEST(ProgramOptionsTest, CanParseBoolFlag) {
  program prog("foo", "A random test command");
  auto command_one = prog.add_subcommand("command-one");
  command_one.add_bool_flag("bar", "Enable the 'bar' feature");
  auto parsed_1 = prog.parse_args({ "command-one", "--bar" });
  ASSERT_TRUE(parsed_1.find("bar") != parsed_1.end());
  ASSERT_TRUE(std::any_cast<bool>(parsed_1["bar"]));
}


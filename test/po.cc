
#include "gtest/gtest.h"

#include "zen/po.hpp"

auto prog = zen::po::program("git", "A fake Git CLI tool")
  .subcommand(
    zen::po::command("remote", "Commands for remote management")
      .subcommand(
        zen::po::command("set-url")
          .pos_arg("URL"))
      .subcommand(
        zen::po::command("get-url"))
          .flag(zen::po::flag<bool>("push", "Query push URLs rather than fetch URLs"))
      .subcommand(
        zen::po::command("remove")
          .pos_arg("NAME"))
    )
  .subcommand(
      zen::po::command("commit", "Record changes to the repository")
    )
  .flag(zen::po::flag<bool>("bare"));

TEST(POTest, ReportsErrorWhenCommandNotFound) {
  auto res = prog.parse_args({ "foobar" });
  ASSERT_TRUE(res.is_left());
  auto& err = res.left();
  ASSERT_TRUE(err.is<zen::po::command_not_found_error>());
  auto real_err = err.as<zen::po::command_not_found_error>();
  ASSERT_EQ(real_err.actual, "foobar");
}

TEST(POTest, ConvertsFlagToBool) {
  auto match = prog
    .parse_args({ "--bare" })
    .unwrap();
  ASSERT_EQ(match.count_flags(), 1);
  ASSERT_TRUE(match.has_flag("bare"));
  auto bare = match.get_flag<bool>("bare");
  ASSERT_TRUE(bare.has_value());
  ASSERT_EQ(*bare, true);
}

TEST(POTest, CanParseSubcommandsNoPositional) {
  auto match = prog
    .parse_args({ "remote", "get-url" })
    .unwrap();
  ASSERT_TRUE(match.has_subcommand());
  auto [name, remote] = match.subcommand();
  ASSERT_EQ(name, "remote");
  ASSERT_TRUE(remote->has_subcommand());
  auto [name2, geturl] = remote->subcommand();
  ASSERT_FALSE(geturl->has_subcommand());
  ASSERT_EQ(geturl->count_pos_args(), 0);
}

TEST(POTest, FailsOnExcessPositional) {
  auto res = prog.parse_args({ "remote", "get-url", "foobar" });
  ASSERT_TRUE(res.is_left());
  ASSERT_TRUE(res.left().is<zen::po::excess_positional_arg_error>());
}


TEST(POTest, CanParseSubcommandsEndingPositional)  {

  auto match = prog
    .parse_args({ "remote", "set-url", "foobar" })
    .unwrap();

  ASSERT_TRUE(match.has_subcommand());
  auto [name, remote] = match.subcommand();
  ASSERT_EQ(name, "remote");
  ASSERT_TRUE(remote->has_subcommand());
  auto [name2, seturl] = remote->subcommand();
  ASSERT_EQ(name2, "set-url");
  ASSERT_FALSE(seturl->has_subcommand());
  ASSERT_EQ(seturl->count_pos_args(), 1);
  ASSERT_EQ(seturl->get_pos_arg(0), "foobar");
}



#include "gtest/gtest.h"

#include "zen/po.hpp"

TEST(POTest, CanParseSubcommands) {

  const char* argv[] = { "/usr/bin/bolt", "test.bolt" };

  zen::program("bolt", "The offical compiler for the Bolt programming language")
    .subcommand(
      zen::command("check", "Check sources for programming mistakes")
        .pos_arg("FILE", zen::some)
        .fallback())
    .subcommand(
      zen::command("eval", "Run sources")
        .pos_arg("FILE", zen::some))
    .parse_args(2, argv);

}

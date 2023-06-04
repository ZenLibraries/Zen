
#include "gtest/gtest.h"

#include "zen/po.hpp"

TEST(POTest, CanParseSubcommands) {

  const char* argv[] = { "/usr/bin/bolt", "test.bolt" };

  zen::po::program("bolt", "The offical compiler for the Bolt programming language")
    .subcommand(
      zen::po::command("check", "Check sources for programming mistakes")
        .pos_arg("FILE", zen::po::some)
        .fallback())
    .subcommand(
      zen::po::command("eval", "Run sources")
        .pos_arg("FILE", zen::po::some))
    .parse_args(2, argv)
    .unwrap();

}

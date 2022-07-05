
#include "zen/either.hpp"

#include "gtest/gtest.h"

TEST(EitherTest, CanCreateAnEitherAndQueryItsValue) {

  zen::either<int, std::string> a = zen::left(1);
  ASSERT_TRUE(a.is_left());
  ASSERT_FALSE(a.is_right());
  ASSERT_EQ(a.left(), 1);

  zen::either<int, std::string> b = zen::right("foo");
  ASSERT_FALSE(b.is_left());
  ASSERT_TRUE(b.is_right());
  ASSERT_EQ(b.right(), "foo");

}

struct AllocInfo {
  int construct_count = 0;
  int destroy_count = 0;
};

struct Counter {

  bool empty = false;
  AllocInfo& info;

  inline Counter(AllocInfo& info):
    info(info) {
      ++info.construct_count;
    }

  inline Counter(Counter&& other):
    info(other.info) {
      other.empty = true;
      empty = false;
    }

  inline Counter(const Counter& other):
    info(other.info) {
      ++info.construct_count;
    }

  ~Counter() {
    if (!empty) {
      ++info.destroy_count;
    }
  }

};

TEST(EitherTest, EiherDestroysItsValue) {

  AllocInfo info;
  {
    zen::either<int, Counter> x = zen::right(Counter(info));
  }
  ASSERT_EQ(info.construct_count, 1);
  ASSERT_EQ(info.destroy_count, 1);

  AllocInfo info2;
  {
    zen::either<Counter, int> x = zen::left(Counter(info2));
  }
  ASSERT_EQ(info2.construct_count, 1);
  ASSERT_EQ(info2.destroy_count, 1);


}

TEST(EitherTest, EitherCanBeMovedToAnotherEither) {
  AllocInfo info;
  {
    zen::either<int, Counter> e1 = zen::right(Counter(info));
    auto e2 = std::move(e1);
    auto e3 = std::move(e2);
    ASSERT_TRUE(e3.is_right());
  }
  ASSERT_EQ(info.construct_count, 1);
  ASSERT_EQ(info.destroy_count, 1);
}

TEST(EitherTest, EitherValueCanBeCopied) {
  AllocInfo info;
  {
    zen::either<int, Counter> e1 = zen::right(Counter(info));
    auto e2 = e1;
    auto e3 = e2;
    ASSERT_TRUE(e3.is_right());
  }
  ASSERT_EQ(info.construct_count, 3);
  ASSERT_EQ(info.destroy_count, 3);
}


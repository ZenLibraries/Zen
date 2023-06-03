
#include <iterator>
#include <type_traits>
#include <vector>

#include "zen/meta.hpp"

using namespace zen::meta;

static_assert(is_pointer_v<char*>);
static_assert(!is_pointer_v<bool>);
static_assert(!is_pointer_v<std::vector<int>>);
static_assert(!is_pointer_v<std::vector<int>::iterator>);

static_assert(!lift<std::is_pointer<_1>>::apply<bool>::value);
static_assert(lift<std::is_pointer<_1>>::apply<bool*>::value);
static_assert(!lift<std::is_pointer<_1>>::apply<std::vector<int>>::value);
static_assert(lift<std::is_pointer<_1>>::apply<int*>::value);

static_assert(std::is_same_v<head_t<std::tuple<bool, int, std::string>>, bool>);
static_assert(std::is_same_v<head_t<std::tuple<int, bool, std::string>>, int>);

static_assert(std::is_same_v<rest_t<std::tuple<int, bool, std::string>>, std::tuple<bool, std::string>>);
static_assert(std::is_same_v<rest_t<std::tuple<std::string>>, std::tuple<>>);

static_assert(is_null<std::tuple<>>::value);
static_assert(!is_null<std::tuple<int>>::value);
static_assert(!is_null<std::tuple<int, float, bool>>::value);

static_assert(index_t<std::tuple<bool, int, std::string>, bool>::value == 0);

static_assert(sz<1>::value == 1);

// TODO make this work and remove it
//      Probably something wrong with eval_t<>

// using tags = std::tuple<
//   std::input_iterator_tag
// , std::forward_iterator_tag
// , std::bidirectional_iterator_tag
// , std::random_access_iterator_tag
// , std::output_iterator_tag
// >;

// template<typename IterT>
// struct _get_iterator_category {
//   using type = std::iterator_traits<IterT>::iterator_category;
// };

// static_assert(
//   std::is_same<
//     min_by_t<
//       lift<index_t<tags, _1>>,
//       std::tuple<std::random_access_iterator_tag, _get_iterator_category<std::vector<int>::iterator>>,
//       std::input_iterator_tag
//     >,
//     std::random_access_iterator_tag
//   >::value)

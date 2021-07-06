
#include <type_traits>
#include <vector>

#include "zen/meta.hpp"

static_assert(zen::meta::is_pointer_v<char*>);
static_assert(!zen::meta::is_pointer_v<bool>);
static_assert(!zen::meta::is_pointer_v<std::vector<int>>);
static_assert(!zen::meta::is_pointer_v<std::vector<int>::iterator>);

static_assert(!zen::meta::lift<std::is_pointer<zen::meta::_1>>::apply<bool>::value);
static_assert(zen::meta::lift<std::is_pointer<zen::meta::_1>>::apply<bool*>::value);
static_assert(!zen::meta::lift<std::is_pointer<zen::meta::_1>>::apply<std::vector<int>>::value);
static_assert(zen::meta::lift<std::is_pointer<zen::meta::_1>>::apply<int*>::value);


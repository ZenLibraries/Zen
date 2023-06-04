
#include <concepts>
#include <ostream>
#include <sstream>

ZEN_NAMESPACE_START

template<typename T>
concept has_display = requires(T value, std::ostream& out) {
  { value.display(out) } -> std::same_as<void>;
};

template<typename T>
std::string display(const T& value);

template<has_display T>
std::string display(const T& value) {
  std::ostringstream out;
  value.display(out);
  return out.str();
}

ZEN_NAMESPACE_END

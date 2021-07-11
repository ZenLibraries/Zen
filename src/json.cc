
#include <cstdlib>
#include <memory>
#include <sstream>
#include <cmath>
#include <stack>
#include <variant>

#include "zen/char.hpp"
#include "zen/config.hpp"
#include "zen/transformer.hpp"
#include "zen/json.hpp"
#include "zen/stream.hpp"
#include "zen/either.hpp"
#include "zen/value.hpp"

ZEN_NAMESPACE_START

static void print_json_string(const string& str, std::ostream& out) {
  out << '"';
  for (auto ch: str) {
    // FIXME This should be UTF-8-encoded
    out << ch;
  }
  out << '"';
}

static void print_impl(const value &v, std::ostream& out, int indent) {

  switch (v.get_type()) {

    case value_type::array:
    {
      auto array = v.as_array();
      if (array.empty()) {
        out << "[]";
        break;
      }
      out << "[\n";
      auto curr = array.cbegin();
      auto end = array.cend();
      if (curr != end) {
        auto new_indent = indent + 2;
        out << std::string(indent + 2, ' ');
        print_impl(*curr, out, new_indent);
        curr++;
        for (; curr != end; curr++) {
          out << ",\n" << std::string(indent + 2, ' ');
          print_impl(*curr, out, indent + 2);
        }
      }
      out << "\n" << std::string(indent, ' ') << "]";
      break;
    }

    case value_type::boolean:
      out << (v.is_true() ? "true" : "false");
      break;

    case value_type::string:
      print_json_string(v.as_string(), out);
      break;

    case value_type::null:
      out << "null";
      break;

    case value_type::fractional:
      out << v.as_fractional();
      break;

    case value_type::integer:
      out << v.as_integer();
      break;

    case value_type::object:
    {
      auto object = v.as_object();
      if (object.empty()) {
        out << "{}";
        break;
      }
      out << "{\n";
      auto curr = object.cbegin();
      auto end = object.cend();
      if (curr != end) {
        auto new_indent = indent + 2;
        out << std::string(new_indent, ' ');
        print_json_string(curr->first, out);
        out << ": ";
        print_impl(curr->second, out, new_indent);
        curr++;
        for (; curr != end; curr++) {
          out << ",\n" << std::string(new_indent, ' ');
          print_json_string(curr->first, out);
          out << ": ";
          print_impl(curr->second, out, new_indent);
        }
      }
      out << "\n" << std::string(indent, ' ') << "}";
      break;
    }

  }

}

void print(const value &v, std::ostream& out) {
  print_impl(v, out, 0);
}

std::string to_string(const value& v) {
  std::ostringstream ss;
  print(v, ss);
  return ss.str();
}

std::string escape_char(char ch) {
  switch (ch) {
    case '\"': return "\\\"";
    case '\\': return "\\\\";
    // case '/': return "\/";
    case '\b': return "\\b";
    case '\f': return "\\f";
    case '\n': return "\\n";
    case '\r': return "\\r";
    case '\t': return "\\t";
    default: return std::string { ch };
  }
}

class json_encoder : public transformer {

  std::stack<bool> levels;
  std::string indentation;

  std::ostream& out;

  void write_indentation(int count) {
    for (auto i = 0; i < count; ++i) {
      out << indentation;
    }
  }

  void write_string(const std::string& str) {
    out << '"';
    for (auto ch: str) {
      out << escape_char(ch);
    }
    out << '"';
  }

public:

  json_encoder(std::ostream& out, std::string indentation):
    indentation(indentation), out(out) {}

  void transform(bool& v) override {
    out << (v ? "true" : "false");
  }

  void transform(char& v) override {
    out << '"' << escape_char(v) << '"';
  }

  void transform(short& v) override {
    out << v;
  }

  void transform(int& v) override {
    out << v;
  }

  void transform(long& v) override {
    out << v;
  }

  void transform(long long& v) override {
    out << v;
  }

  void transform(unsigned char& v) override {
    out << v;
  }

  void transform(unsigned short& v) override {
    out << v;
  }

  void transform(unsigned int& v) override {
    out << v;
  }

  void transform(unsigned long& v) override {
    out << v;
  }

  void transform(unsigned long long& v) override {
    out << v;
  }

  void transform(float& v) override {
    float integral;
    if (std::modf(v, &integral) == 0) {
      out << integral << ".0";
    } else {
      out << v << v;
    }
  }

  void transform(double& v) override {
    double integral;
    if (std::modf(v, &integral) == 0) {
      out << integral << ".0";
    } else {
      out << v << v;
    }
  }

  void transform(std::string& v) override {
    write_string(v);
  }

  void start_transform_object(const std::string& tag_name) override {
    out << "{";
    levels.push(true);
  }

  void end_transform_object() override {
    levels.pop();
    if (!indentation.empty()) {
      out << "\n";
      write_indentation(levels.size());
    }
    out << "}";
  }

  void start_transform_field(const std::string& name) override {
    if (!levels.top()) {
      out << ",";
    } else {
      levels.top() = false;
    }
    if (!indentation.empty()) {
      out << "\n";
      write_indentation(levels.size());
    }
    write_string(name);
    out << ":";
    if (!indentation.empty()) {
      out << " ";
    }
  }

  void end_transform_field() override {

  }

  void start_transform_element() override {
    if (!levels.top()) {
      out << ",";
    } else {
      levels.top() = false;
    }
    if (!indentation.empty()) {
      out << "\n";
      write_indentation(levels.size());
    }
  }

  void end_transform_element() override {
    // if (!indentation.empty()) {
    //   out << "\n";
    //   write_indentation(levels.size());
    // }
  }

  void start_transform_optional() override {

  }

  void end_transform_optional() override {

  }

  void start_transform_sequence() override {
    levels.push(true);
    out << "[";
  }

  void end_transform_sequence() override {
    levels.pop();
    if (!indentation.empty()) {
      out << "\n";
      write_indentation(levels.size());
    }
    out << "]";
  }

  void transform_nil() override {
    out << "null";
  }

  void transform_size(std::size_t size) override {

  }

  ~json_encoder() {

  }

};

std::unique_ptr<transformer> make_json_encoder(
  std::ostream& out,
  json_encode_opts opts
) {
  return std::make_unique<json_encoder>(out, opts.indentation);
}

static bool is_json_whitespace(char ch) {
  switch (ch) {
    case ' ':
    case '\n':
    case '\r':
    case '\t':
      return true;
    default:
      return false;
  }
}

static bool is_json_digit(char ch) {
  switch (ch) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return true;
    default:
      return false;
  }
}

json_parse_result parse_json(std::istream& in) {

  value result;
  std::optional<string> key;
  std::stack<value> building;

  for (;;) {

    int c0;

#define ZEN_GET_NO_WHITESPACE(ch) \
    for (;;) { \
      ch = in.get(); \
      if (!is_json_whitespace(ch)) { \
        break; \
      } \
    }

#define ZEN_PEEK_NO_WHITESPACE(ch) \
    for (;;) { \
      ch = in.peek(); \
      if (!is_json_whitespace(ch)) { \
        break; \
      } \
      in.get(); \
    }

#define ZEN_SCAN_DIGIT(name) \
  unsigned char name; \
  { \
    auto ch = in.get(); \
    if (!is_json_digit(ch)) { \
      return zen::left(json_parse_error::unexpected_character); \
    } \
    name = parse_decimal_digit(ch); \
  }

#define ZEN_ASSERT_CHAR(ch, expected) \
    { \
      auto temp = ch; \
      if (temp != expected) { \
        return left(json_parse_error::unexpected_character); \
      } \
    }

#define ZEN_EXPECT_CHAR(expected) \
  ZEN_ASSERT_CHAR(in.get(), expected);

  ZEN_GET_NO_WHITESPACE(c0);

  switch (c0) {

      case '{':
        building.push(object {});
        continue;

      case ']':
      case '}':
        result = building.top();
        building.pop();
        break;

      case '[':
        building.push(array {});
        continue;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        bigint x = parse_decimal_digit(c0);

        for (;;) {

          c0 = in.peek();

          switch (c0) {

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              in.get();
              x = x * 10 + parse_decimal_digit(c0);
              continue;

            case '.':
            {
              in.get();
              fractional e = 1.0;
              fractional y = 0.0;
              fractional k = 1.0;

              for (;;) {

                auto c1 = in.peek();

                switch (c1) {

                  case '0':
                  case '1':
                  case '2':
                  case '3':
                  case '4':
                  case '5':
                  case '6':
                  case '7':
                  case '8':
                  case '9':
                    in.get();
                    k /= 10.0;
                    y += k * parse_decimal_digit(c1);
                    continue;

                  case 'e':
                  case 'E':
                  {
                    in.get();
                    e = 0.0;
                    for (;;) {
                      auto c2 = in.peek();
                      switch (c2) {
                        case '+':
                          in.get();
                          break;
                        case '-':
                          in.get();
                          e *= -1;
                          break;
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                          in.get();
                          e = e * 10.0 + parse_decimal_digit(c2);
                          break;
                        case EOF:
                        case ']':
                        case '}':
                        case ',':
                        case ' ':
                        case '\t':
                        case '\r':
                        case '\n':
                          goto finish_fractional;
                        default:
                          return left(json_parse_error::unexpected_character);
                      }
                    }
                  }

                  case EOF:
                  case ']':
                  case '}':
                  case ',':
                  case ' ':
                  case '\t':
                  case '\r':
                  case '\n':
                    goto finish_fractional;

                  default:
                    return left(json_parse_error::unexpected_character);

                }

              }

finish_fractional:
              result = value(std::pow(x + y, e));
              goto process_result;

            }

            case ']':
            case '}':
            case ',':
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            case EOF:
              goto finish_integer;

            default:
              return left(json_parse_error::unexpected_character);

          }

        }

finish_integer:
        result = value(x);
        break;
      }

      case '"':
      {
        string chars;
        for (;;) {
          auto c1 = in.get();
          switch (c1) {
            case '"':
              goto finish_string;
            case '\n':
              return left(json_parse_error::unexpected_character);
            case '\\':
            {
              auto c2 = in.get();
              switch (c2) {
                case '"':
                  chars.push_back('"');
                  break;
                case '\\':
                  chars.push_back('\\');
                  break;
                case '/':
                  chars.push_back('/');
                  break;
                case 'b':
                  chars.push_back('\b');
                  break;
                case 'f':
                  chars.push_back('\f');
                  break;
                case 'n':
                  chars.push_back('\n');
                  break;
                case 'r':
                  chars.push_back('\r');
                  break;
                case 't':
                  chars.push_back('\t');
                  break;
                case 'u':
                  ZEN_SCAN_DIGIT(d0);
                  ZEN_SCAN_DIGIT(d1);
                  ZEN_SCAN_DIGIT(d2);
                  ZEN_SCAN_DIGIT(d3);
                  chars.push_back(d0 * 1000 + d1 * 100 + d2 * 10 + d3);
                  break;
                default:
                  return left(json_parse_error::unrecognised_escape_sequence);
              }
              break;
            }
            default:
              chars.push_back(c1);
              break;
          }
        }
finish_string:
        if (!building.empty() && building.top().is_object() && !key.has_value()) {
          key = chars;
          ZEN_GET_NO_WHITESPACE(c0)
          ZEN_ASSERT_CHAR(c0, ':');
          continue;
        }
        result = value(chars);
        break;
      }

      case 'n':
        ZEN_EXPECT_CHAR('u');
        ZEN_EXPECT_CHAR('l');
        ZEN_EXPECT_CHAR('l');
        result = value(null {});
        break;

      case 't':
        ZEN_EXPECT_CHAR('r');
        ZEN_EXPECT_CHAR('u');
        ZEN_EXPECT_CHAR('e');
        result = value(true);
        break;

      case 'f':
        ZEN_EXPECT_CHAR('a');
        ZEN_EXPECT_CHAR('l');
        ZEN_EXPECT_CHAR('s');
        ZEN_EXPECT_CHAR('e');
        result = value(false);
        break;

      default:
        return left(json_parse_error::unexpected_character);

    }

process_result:

    if (building.empty()) {
      break;
    }

    auto& top = building.top();

    switch (top.get_type()) {

      case value_type::object:
        top.as_object().emplace(*key, result);
        key = {};
        break;

      case value_type::array:
        top.as_array().push_back(result);
        break;

      default:
        ZEN_UNREACHABLE

    }

    ZEN_PEEK_NO_WHITESPACE(c0)
    switch (c0) {
      case '}':
      case ']':
        result = building.top();
        building.pop();
        goto process_result;
      case ',':
        in.get();
        continue;
      default:
        return zen::left(json_parse_error::unexpected_character);
    }

    // We should never be able to get here, as the previous switch-statement
    // should have modified the control-flow in all cases.

  }

  return right(result);

}

json_parse_result parse_json(const std::string& in) {
  std::istringstream iss(in);
  return parse_json(iss);
}

// std::unique_ptr<transformer> make_json_decoder(
//   std::istream& in,
//   json_decode_opts opts
// ) {
//   return std::make_unique<json_decoder>(in);
// }

ZEN_NAMESPACE_END


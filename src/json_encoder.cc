
#include <memory>
#include <sstream>
#include <cmath>
#include <stack>

#include "zen/encoder.hpp"

ZEN_NAMESPACE_START

    //static void print_json_string(std::string str, std::ostream& out) {
    //  out << '"';
    //  for (auto ch: str) {
    //    out << ch;
    //  }
    //  out << '"';
    //}

    //static void print_impl(const value &v, std::ostream& out, int indent) {

    //  switch (v.get_type()) {

    //    case value_type::array:
    //    {
    //      auto array = v.get_array();
    //      if (array.empty()) {
    //        out << "[]";
    //        break;
    //      }
    //      out << "[\n";
    //      auto curr = array.cbegin();
    //      auto end = array.cend();
    //      if (curr != end) {
    //        auto new_indent = indent + 2;
    //        out << std::string(indent + 2, ' ');
    //        print_impl(**curr, out, new_indent);
    //        curr++;
    //        for (; curr != end; curr++) {
    //          out << ",\n" << std::string(indent + 2, ' ');
    //          print_impl(**curr, out, indent + 2);
    //        }
    //      }
    //      out << "\n" << std::string(indent, ' ') << "]";
    //      break;
    //    }

    //    case value_type::boolean:
    //      out << (v.is_true() ? "true" : "false");
    //      break;

    //    case value_type::string:
    //      print_json_string(v.get_string(), out);
    //      break;

    //    case value_type::null:
    //      out << "null";
    //      break;

    //    case value_type::dooble:
    //      out << v.get_double();
    //      break;

    //    case value_type::integer:
    //      out << v.get_long();
    //      break;

    //    case value_type::object:
    //    {
    //      auto object = v.get_object();
    //      if (object.empty()) {
    //        out << "{}";
    //        break;
    //      }
    //      out << "{\n";
    //      auto curr = object.cbegin();
    //      auto end = object.cend();
    //      if (curr != end) {
    //        auto new_indent = indent + 2;
    //        out << std::string(new_indent, ' ');
    //        print_json_string(curr->first, out);
    //        out << ": ";
    //        print_impl(*curr->second, out, new_indent);
    //        curr++;
    //        for (; curr != end; curr++) {
    //          out << ",\n" << std::string(new_indent, ' ');
    //          print_json_string(curr->first, out);
    //          out << ": ";
    //          print_impl(*curr->second, out, new_indent);
    //        }
    //      }
    //      out << "\n" << std::string(indent, ' ') << "}";
    //      break;
    //    }

    //  }

    //}

    //void print(const value &v, std::ostream& out) {
    //  print_impl(v, out, 0);
    //}

    //std::string to_string(const value& v) {
    //  std::ostringstream ss;
    //  print(v, ss);
    //  return ss.str();
    //}

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

class json_encoder : public encoder {

  std::stack<bool> levels;
  std::string indentation;

  std::ostream& out;

  void write_indentation(int count) {
    for (auto i = 0; i < count; ++i) {
      out << indentation;
    }
  }

public:

  json_encoder(std::ostream& out, std::string indentation):
    indentation(indentation), out(out) {}

  void encode(bool v) override {
    out << (v ? "true" : "false");
  }

  void encode(char v) override {
    out << '"' << escape_char(v) << '"';
  }

  void encode(short v) override {
    out << v;
  }

  void encode(int v) override {
    out << v;
  }

  void encode(long v) override {
    out << v;
  }

  void encode(long long v) override {
    out << v;
  }

  void encode(unsigned char v) override {
    out << v;
  }

  void encode(unsigned short v) override {
    out << v;
  }

  void encode(unsigned int v) override {
    out << v;
  }

  void encode(unsigned long v) override {
    out << v;
  }

  void encode(unsigned long long v) override {
    out << v;
  }

  void encode(float v) override {
    float integral;
    if (std::modf(v, &integral) == 0) {
      out << integral << ".0";
    } else {
      out << v << v;
    }
  }

  void encode(double v) override {
    double integral;
    if (std::modf(v, &integral) == 0) {
      out << integral << ".0";
    } else {
      out << v << v;
    }
  }

  void encode(const std::string_view& v) override {
    out << '"';
    for (auto ch: v) {
      out << escape_char(ch);
    }
    out << '"';
  }

  void start_encode_struct(std::string tag_name) override {
    out << "{";
    levels.push(true);
  }

  void end_encode_struct() override {
    levels.pop();
    if (!indentation.empty()) {
      out << "\n";
      write_indentation(levels.size());
    }
    out << "}";
  }

  void start_encode_field(std::string name) override {
    if (!levels.top()) {
      out << ",";
    } else {
      levels.top() = false;
    }
    if (!indentation.empty()) {
      out << "\n";
      write_indentation(levels.size());
    }
    encode(name);
    out << ":";
    if (!indentation.empty()) {
      out << " ";
    }
  }

  void end_encode_field() override {

  }

  void start_encode_element() override {
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

  void end_encode_element() override {
    // if (!indentation.empty()) {
    //   out << "\n";
    //   write_indentation(levels.size());
    // }
  }

  void start_encode_optional() override {
  }

  void end_encode_optional() override {
  }

  void start_encode_sequence() override {
    levels.push(true);
    out << "[";
  }

  void end_encode_sequence() override {
    levels.pop();
    if (!indentation.empty()) {
      out << "\n";
      write_indentation(levels.size());
    }
    out << "]";
  }

  void encode_nil() override {
    out << "null";
  }

  void encode_size(std::size_t size) override {

  }

  ~json_encoder() {

  }

};

std::unique_ptr<encoder> make_json_encoder(std::ostream& out, json_encode_opts opts) {
  return std::make_unique<json_encoder>(out, opts.indentation);
}

ZEN_NAMESPACE_END


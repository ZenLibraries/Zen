#ifndef ZEN_JSON_HPP
#define ZEN_JSON_HPP

#include <memory>
#include <istream>
#include <ostream>

#include "zen/transformer.hpp"
#include "zen/value.hpp"
#include "zen/either.hpp"

ZEN_NAMESPACE_START

enum class json_token_type {
  end_of_file,
  integer,
  fractional,
  array_start,
  array_end,
  object_start,
  object_end,
  comma,
  colon,
  kw_false,
  kw_true,
  kw_null,
  identifier,
};

enum class json_parse_error {
  unrecognised_escape_sequence,
  unexpected_character,
};

using json_parse_result = either<json_parse_error, value>;

json_parse_result parse_json(std::istream& in);
json_parse_result parse_json(const std::string& in);

struct json_encode_opts {
  std::string indentation = "  ";
};

std::unique_ptr<transformer> make_json_decoder(
  std::istream& input,
  json_encode_opts opts = {}
);

struct json_decode_opts {

};

std::unique_ptr<transformer> make_json_encoder(
  std::ostream& output,
  json_decode_opts opts = {}
);

template<typename InputT, typename T>
void decode_json(InputT input, T& value) {
  auto decoder = make_json_decoder(input);
  decode(decoder, value);
}

template<typename OutputT, typename T>
void encode_json(OutputT output, T& value) {
  auto encoder = make_json_encoder(output);
  decode(encoder, value);
}

ZEN_NAMESPACE_END

#endif // of #ifndef ZEN_JSON_HPP

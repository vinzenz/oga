//
// Copyright 2014 Vinzenz Feenstra, Red Hat, Inc. and/or its affiliates.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Refer to the README and COPYING files for full details of the license.
//

#ifndef GUARD_OGA_PROTO_JSON_JSONPARSER_HPP_INCLUDED
#define GUARD_OGA_PROTO_JSON_JSONPARSER_HPP_INCLUDED

#include <oga/proto/json/json.hpp>

namespace oga {
namespace proto {
namespace json {

enum token_type {
    tt_invalid,
    tt_end,
    tt_sep,
    tt_null,
    tt_object_start,
    tt_key,
    tt_value,      //
    tt_object_sep, // :
    tt_object_end, // }
    tt_array_start,// [
    tt_array_end,  // ]
    tt_string,     // "
    tt_boolean,    // t/f
    tt_number,     // -/0/1/2/3/4/5/6/7/8/9

    // Always last
    tt_token_count
};

struct token {
    token_type type;
    token_type usage;
    char const * start;
    char const * end;
};

struct context {
    char const * start;
    char const * end;
    char const * current;
    std::vector<token> tokens;
    std::string (*string_filter_hook)(std::string);
};


bool parse(char const * start, char const * end, value & result);
bool parse_filter(char const * start, char const * end, value & result, std::string (*hook)(std::string));
token_type advance(context & ctx);
bool skipws(context & ctx);
bool parse_value(context & ctx, value & result, token_type usage = tt_invalid);
bool parse_object(context & ctx, value & result, token_type usage = tt_invalid);
bool parse_array(context & ctx, value & result, token_type usage = tt_invalid);
bool parse_boolean(context & ctx, value & result, token_type usage = tt_invalid);
bool parse_null(context & ctx, value & result, token_type usage = tt_invalid);
bool parse_number(context & ctx, value & result, token_type usage = tt_invalid);
bool parse_string(context & ctx, value & result, token_type usage = tt_invalid);

}}}

#endif //GUARD_OGA_PROTO_JSON_JSONPARSER_HPP_INCLUDED


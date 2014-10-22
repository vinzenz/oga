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

#include <oga/proto/json/json_parser.hpp>
#if defined(OGA_PROTO_JSON_GENERATE_TEST)
#include <oga/proto/json/json_generator.hpp>
#endif
#include <stdio.h>
#include <string.h>
#include <string>
using namespace oga::proto::json;
using std::string;

typedef bool (*parse_fun_t)(context &, value &, token_type);

void dump(std::vector<token> & tokens) {
    for(size_t i = 0; i < tokens.size(); ++i) {
        token const & tok = tokens[i];
        printf("Token::type: %d\n", tok.type);
        printf("Token::usage: %d\n", tok.usage);
        printf("Token match: %s\n", string(tok.start, tok.end).c_str());
    }
}

struct dumper{
    void operator()(value const & v) const {
        apply_visitor(*this, v);
    }

    void operator()(array const & a) const {
        for(size_t i = 0; i < a.size(); ++i) {
            (*this)(a[i]);
        }
    }

    void operator()(int64_t i) const {
        printf("%lli", i);
    }

    void operator()(object const & o) const {
        printf("{\n");
        for(object::const_iterator it = o.begin(), end = o.end(); it != end; ++it) {
            object::value_type const & p = *it;
            printf("%s: ", p.first.c_str());
            (*this)(p.second);
            printf(",\n");
        }
        printf("}");
    }

    void operator()(std::string const & s) const {
        printf("%s", s.c_str());
    }

    void operator()(null const & n) const {
        printf("null");
    }

    void operator()(double d) const {
        printf("%e", d);
    }

    void operator()(bool b) const {
        printf("%s", b ? "true" : "false");
    }

};

struct test_case {
    char const * input;
    parse_fun_t  fun;
    bool expected;
};

void test(test_case const & tc,  bool is_parse_value) {
    char const * str = tc.input;
    parse_fun_t fun = is_parse_value ? parse_value : tc.fun;
    printf("-----------------------------\n");
    printf("Parsed string: `%s`\n", str);
    printf("Using parse_value: %s\n", is_parse_value ? "Yes" : "No");
    context ctx{str, str + strlen(str), str, {}};
    value result;
    if(fun(ctx, result, tt_invalid) == tc.expected) {
        printf("Result: success\n");
        dump(ctx.tokens);
        printf("\nDumping parse result:\n");
        apply_visitor(dumper(), result);
        printf("\n");
#if defined(OGA_PROTO_JSON_GENERATE_TEST)
        printf("Generated: `%s`\n", generate(result).c_str());
#endif
    }
    else {
        printf("Result: fail\n");
    }
}

test_case const cases[] = {
    {"\"abcdefgh\"", parse_string, true},
    {"\"abcde\\\\\\/\\u0AabF\\\"fgh\"", parse_string, true},
    {"\"\\u05E9\\u05E7\"", parse_string, true},
    {"-9.33e22", parse_number, true},
    {"-.33e22", parse_number, false},
    {"-33e22", parse_number, true},
    {"-123456789", parse_number, true},
    {"123456789", parse_number, true},
    {"true", parse_boolean, true},
    {"false", parse_boolean, true},
    {"null", parse_null, true},
    {"{}", parse_object, true},
    {"[]", parse_array, true},
    {0, 0, false}
};

int main() {
    test_case const * tc = cases;
    for(;tc->input != 0;++tc) {
        test(*tc, false);
        test(*tc, true);
    }
}


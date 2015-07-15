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
#include <utf8/checked.h>
#include <string.h>
#include <stdlib.h>

namespace oga {
namespace proto {
namespace json {

int64_t make_int(char const * start, char const * end) {
    int64_t result = 0;
    bool neg = start != end && *start == '-';
    if(neg) ++start;
    while(start != end) {
        result *= 10;
        result += (*start - '0');
        ++start;
    }
    if(neg) {
        result = -result;
    }
    return result;
}

double make_double(char const * start, char const * end)
{
    if(end - start < 64) {
        char buffer[64] = {};
        memcpy(buffer, start, end - start);
        return strtod(buffer, 0);
    }
    std::string tmp(start, end);
    return strtod(tmp.c_str(), 0);
}

bool parse(char const * start, char const * end, value & result) {
    context ctx;
    ctx.start = start;
    ctx.end = end;
    ctx.current = start;
    ctx.string_filter_hook = 0;
    return parse_value(ctx, result);
}

bool parse_filter(char const * start, char const * end, value & result, std::string (*hook)(std::string)) {
    context ctx;
    ctx.start = start;
    ctx.end = end;
    ctx.current = start;
    ctx.string_filter_hook = hook;
    return parse_value(ctx, result);
}

inline void push(context & ctx, token_type type, char const *start, token_type usage = tt_invalid) {
    token tok = {type, usage, start, ctx.current};
    ctx.tokens.push_back(tok);
}

inline char next_ch(context & ctx) {
    if(++ctx.current != ctx.end) {
        return *ctx.current;
    }
    return 0;
}

inline char next_ch_cur(context & ctx) {
    char ch = 0;
    if(ctx.current != ctx.end) {
        ch = *ctx.current;
    }
    return ch;
}

token_type advance(context & ctx) {
    if(skipws(ctx)) {
        char ch = next_ch_cur(ctx);
        if(isdigit(ch)) return tt_number;
        switch(ch) {
        case '[':
            return tt_array_start;
        case ']':
            return tt_array_end;
        case '{':
            return tt_object_start;
        case '}':
            return tt_object_end;
        case ',':
            return tt_sep;
        case '-':
            return tt_number;
        case 'n': case 'N':
            return tt_null;
        case '"':
            return tt_string;
        case ':':
            return tt_object_sep;
        case 'f':
        case 't':
            return tt_boolean;
        default:
            return tt_invalid;
        }
    }
    return tt_end;
}

bool skipws(context & ctx) {
    bool found = true;
    while(found && next_ch_cur(ctx)) {
        switch(*ctx.current) {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                ctx.current++;
                break;
            default:
                found = false;
                break;
        }
    }
    return ctx.current != ctx.end;
}

bool parse_value(context & ctx, value & result, token_type usage) {
    token_type tok = advance(ctx);
    bool ok = false;
    switch(tok) {
    case tt_string:
        ok = parse_string(ctx, result, usage);
        break;
    case tt_number:
        ok = parse_number(ctx, result, usage);
        break;
    case tt_null:
        ok = parse_null(ctx, result, usage);
        break;
    case tt_boolean:
        ok = parse_boolean(ctx, result, usage);
        break;
    case tt_array_start:
        ok = parse_array(ctx, result, usage);
        break;
    case tt_object_start:
        ok = parse_object(ctx, result, usage);
        break;
    default:
        break;
    }
    return ok;
}

bool parse_object(context & ctx, value & result, token_type usage) {
    push(ctx, tt_object_start, ctx.current++, usage);
    bool ok = true;
    value vo = object();
    object & o = vo.get_object();
    while(ok) {
        ok = advance(ctx) == tt_string;
        value key;
        if(ok) {
            ok = parse_string(ctx, key, tt_key);
        }
        if(ok) {
            ok = advance(ctx) == tt_object_sep;
        }
        value val;
        if(ok) {
            ctx.current++;
            ok = parse_value(ctx, val, tt_value);
        }
        if(ok) {
            o[key.get_string()] = val;
        }
        token_type next = advance(ctx);
        if(next == tt_object_end) {
            result.swap(vo);
            push(ctx, tt_object_end, ctx.current++, usage);
            ok = true;
            break;
        }
        if(next != tt_sep) {
            ok = false;
            break;
        }
        push(ctx, tt_sep, ctx.current++);
    }
    return ok;
}

bool parse_array(context & ctx, value & result, token_type) {
    push(ctx, tt_array_start, ctx.current++);
    bool ok = true;
    value va = array();
    array & a = va.get_array();
    while(ok) {
        value entry;
        ok = parse_value(ctx, entry);
        if(ok) {
            a.push_back(entry);
        }
        token_type next = advance(ctx);
        if(next != tt_sep) {
            ok = next == tt_array_end;
            if(ok) {
                result.swap(va);
                push(ctx, tt_array_end, ctx.current++);
            }
            break;
        }
        push(ctx, tt_sep, ctx.current++);
    }
    return ok;
}

bool parse_null(context & ctx, value & result, token_type usage) {
    char const * start = ctx.current;
    bool ok =
           next_ch_cur(ctx) == 'n'
        && next_ch(ctx) == 'u'
        && next_ch(ctx) == 'l'
        && next_ch(ctx) == 'l'
    ;
    if(ok) {
        next_ch(ctx);
        result = null();
        push(ctx, tt_null, start, usage);
    }
    return ok;
}

bool parse_boolean(context & ctx, value & result, token_type usage) {
    char const * start = ctx.current;
    bool ok = false;
    char ch = next_ch_cur(ctx);
    switch(ch) {
    case 'f':
        ok =   next_ch(ctx) == 'a'
            && next_ch(ctx) == 'l'
            && next_ch(ctx) == 's'
            && next_ch(ctx) == 'e';
        result = false;
        break;
    case 't':
        ok =   next_ch(ctx) == 'r'
            && next_ch(ctx) == 'u'
            && next_ch(ctx) == 'e';
        result = true;
        break;
    default:
        break;
    }
    if(ok) {
        next_ch(ctx);
        push(ctx, tt_boolean, start, usage);
    }
    return ok;
}

bool parse_number(context & ctx, value & result, token_type usage) {
    char const * start = ctx.current;
    char ch = next_ch_cur(ctx);
    char first_num = 0;
    bool is_real = false;
    if(ch == '-') ch = next_ch(ctx);
    if(ch != '0' && isdigit(ch)) {
        first_num = ch;
       do {
           ch = next_ch(ctx);
       } while(isdigit(ch));
    }
    if(ch == '0') {
        first_num = ch;
        ch = next_ch(ctx);
    }
    if(first_num && ch == '.') {
        is_real = true;
       do {
           ch = next_ch(ctx);
       } while(isdigit(ch));
    }
    if(ch == 'e' || ch == 'E') {
        is_real = true;
        ch = next_ch(ctx);
        if(ch == '-' || ch == '+') {
            ch = next_ch(ctx);
        }
       do {
           ch = next_ch(ctx);
       } while(isdigit(ch));
    }
    bool ok = start != ctx.current
           && !(!first_num && *start == '-');
    if(ok) {
        if(is_real){
            result = make_double(start, ctx.current);
        }
        else {
            result = make_int(start, ctx.current);
        }
        push(ctx, tt_number, start, usage);
    }
    return ok;
}

inline char next_hex(context & ctx) {
    char ch = next_ch(ctx);
    if( (ch >= 'A' && ch <= 'F')
        || (ch >= 'a' && ch <= 'f')
        || isdigit(ch) )
        return ch;
    return 0;
}

inline char hex_to_num(char c) {
    if(isdigit(c))
        return c - '0';
    if(c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    if(c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    return 0xF;
}

inline uint16_t uni_hex_to_char16(char c1, char c2, char c3, char c4) {
    uint16_t result = uint16_t(hex_to_num(c1));
    result <<= 4;
    result |= hex_to_num(c2);
    result <<= 4;
    result |= hex_to_num(c3);
    result <<= 4;
    result |= hex_to_num(c4);
    return result;
}

inline void u16tou8(std::string & target, uint16_t c) {
    try {
        utf8::utf16to8(&c, (&c) + 1, std::back_inserter(target));
    }
    catch(...) {
        target += '?';
    }
}

bool parse_string(context & ctx, value & result, token_type usage) {
    char const * start = ctx.current;
    char ch = next_ch_cur(ctx);
    std::string target;
    char ubuf[4] = {};
    if(ch == '"') {
        while((ch = next_ch(ctx)) && ch != '"') {
            if(ch == '\\') {
                if(!(ch = next_ch(ctx))) {
                    break;
                }
                bool fail = false;
                switch(ch) {
                case '"': target += '"'; break;
                case 'b': target += '\b'; break;
                case 'n': target += '\n'; break;
                case 'f': target += '\f'; break;
                case 'r': target += '\r'; break;
                case 't': target += '\t'; break;
                case '/': target += '/'; break;
                case '\\': target += '\\'; break;
                case'u':
                    fail = !(ubuf[0] = next_hex(ctx));
                    if(!ubuf[0]) break;
                    fail = !(ubuf[1] = next_hex(ctx));
                    if(!ubuf[1]) break;
                    fail = !(ubuf[2] = next_hex(ctx));
                    if(!ubuf[2]) break;
                    fail = !(ubuf[3] = next_hex(ctx));
                    if(!ubuf[3]) break;
                    u16tou8(target, uni_hex_to_char16(ubuf[0],ubuf[1],ubuf[2],ubuf[3]));
                    break;
                }
                if(fail) break;
            }
            else {
                target += ch;
            }
        }
    }
    bool ok = false;
    if(ch == '"') {
        ++start;
        if(ctx.string_filter_hook) {
            ctx.string_filter_hook(target);
        }
        result = target;
        push(ctx, tt_string, start, usage);
        ctx.current++;
        ok = true;
    }
    return ok;
}

}}}

//
// Copyright 2014-2016 Vinzenz Feenstra, Red Hat, Inc. and/or its affiliates.
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

#include <oga/proto/json/json_generator.hpp>
#include <utf8/checked.h>
#include <cstdio>

#if !defined(_MSC_VER)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
#   define __STDC_FORMAT_MACROS 1
#   include <inttypes.h>
#else // defined(_MSC_VER)
#   if !defined(PRIi64)
#       define PRIi64 "I64d"
#   endif
#endif

#if !defined(PRIi64)
#   error PRIi64 not defined
#endif

namespace oga {
namespace proto {
namespace json {
namespace {
    struct generation_visitor {
        std::string * result;
        generation_visitor(std::string & s)
        : result(&s)
        {}

        void operator()(int64_t v) const {
            char buf[0x100] = {};
            std::snprintf(buf, sizeof(buf), "%" PRIi64, v);
            result->append(buf);
        }

        void operator()(double d) const {
            char buf[0x100] = {};
            snprintf(buf, sizeof(buf), "%.17g", d);
            result->append(buf);
        }

        void operator()(std::string const & s) const {
            result->append("\"");
            typedef  utf8::iterator<std::string::const_iterator> conv_iter_t;
            conv_iter_t iter(s.begin(), s.begin(), s.end()), end(s.end(), s.begin(), s.end());
            while(iter != end) {
                uint32_t cp = *iter;
                switch(cp) {
                case '\b':  result->append("\\b"); break;
                case '\f':  result->append("\\f"); break;
                case '\n':  result->append("\\n"); break;
                case '\r':  result->append("\\r"); break;
                case '\t':  result->append("\\t"); break;
                case '"':   result->append("\\\""); break;
                case '/':   result->append("\\/"); break;
                case '\\':  result->append("\\\\"); break;
                case ' ':   result->append(" "); break;
                default:
                    if(cp < 0x80) {
                        result->append(&cp, &cp + 1);
                    }
                    else {
                        char buf[0x10] = {};
                        if (cp > 0xffff) { //make a surrogate pair
                            uint16_t first = static_cast<uint16_t>((cp >> 10)   + utf8::internal::LEAD_OFFSET);
                            snprintf(buf, sizeof(buf), "%.04x", first);
                            result->append("\\u");
                            result->append(buf);

                            uint16_t second = static_cast<uint16_t>((cp & 0x3ff) + utf8::internal::TRAIL_SURROGATE_MIN);
                            snprintf(buf, sizeof(buf), "%.04x", second);
                            result->append("\\u");
                            result->append(buf);
                        }
                        else  {
                            uint16_t only = static_cast<uint16_t>(cp);
                            snprintf(buf, sizeof(buf), "%.04x", only);
                            result->append("\\u");
                            result->append(buf);
                        }
                    }
                    break;
                }
                ++iter;
            }
            result->append("\"");
        }

        void operator()(null const &) const {
            result->append("null");
        }

        void operator()(bool b) const {
            result->append(b ? "true" : "false");
        }

        void operator()(array const & a) const {
            bool first = true;
            result->append("[");
            for(array::const_iterator item = a.begin(); item != a.end(); item++) {
                if(!first) result->append(",");
                first = false;
                apply_visitor(*this, *item);
            }
            result->append("]");
        }

        void operator()(object const & o) const {
            bool first = true;
            result->append("{");
            for(object::const_iterator item = o.begin(); item != o.end(); item++) {
                if(!first) result->append(",");
                first = false;
                (*this)(item->first);
                result->append(":");
                apply_visitor(*this, item->second);
            }
            result->append("}");
        }
    };
}

std::string generate(value const & val) {
    std::string r;
    generation_visitor v(r);
    apply_visitor(v, val);
    return r;
}

}}}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
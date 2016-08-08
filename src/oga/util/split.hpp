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

#ifndef GUARD_OGA_UTIL_SPLIT_HPP_INCLUDED
#define GUARD_OGA_UTIL_SPLIT_HPP_INCLUDED

#include <vector>
#include <string>

namespace oga {
namespace util {

template< typename CharType >
std::vector< std::basic_string<CharType> > split(std::basic_string<CharType> const & s, CharType sep) {
    typedef std::basic_string<CharType> string_type;
    std::vector<string_type> result;

    size_t last = 0;
    for(size_t pos = 0; pos < s.size(); ++pos) {
        if(s[pos] == sep) {
            result.push_back(string_type(s.c_str() + last, s.c_str() + pos));
            if(result.back().empty()) {
                result.pop_back();
            }
            last = pos + 1;
        }
    }
    result.push_back(string_type(s.c_str() + last, s.c_str() + s.size()));
    if(result.back().empty()) {
        result.pop_back();
    }
    return result;
}

}}

#endif //GUARD_OGA_UTIL_SPLIT_HPP_INCLUDED

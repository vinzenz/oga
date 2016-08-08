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

#ifndef GUARD_OGA_UTIL_STRIP_HPP_INCLUDED
#define GUARD_OGA_UTIL_STRIP_HPP_INCLUDED

namespace oga {
namespace util {

template<typename CharType>
inline CharType last(std::basic_string<CharType> const & s) {
    return s[s.size() - 1];
}

template<typename CharType>
inline CharType first(std::basic_string<CharType> const & s) {
    return s[0];
}

template< typename CharType >
std::basic_string<CharType> const & whitespaces() {
    static const CharType wschars[] = {' ', '\n', '\t', '\r', 0};
    static const std::basic_string<CharType> w = wschars;
    return w;
}

template< typename CharType >
std::basic_string<CharType> & rstrip(std::basic_string<CharType> & s, std::basic_string<CharType> const & strip_chars) {
    while(!s.empty() && strip_chars.find_first_of(last(s)) != std::basic_string<CharType>::npos) {
        s.erase(s.size() - 1);
    }
    return s;
}

template< typename CharType >
std::basic_string<CharType> & rstrip(std::basic_string<CharType> & s) {
    return rstrip(s, whitespaces<CharType>());
}

template< typename CharType >
std::basic_string<CharType> & lstrip(std::basic_string<CharType> & s, std::basic_string<CharType> const & strip_chars) {
    size_t start = 0;
    while(!s.empty() && strip_chars.find_first_of(s[start]) != std::basic_string<CharType>::npos) {
        ++start;
    }
    if(start > 0) {
        s.erase(0, start);
    }
    return s;
}

template< typename CharType >
std::basic_string<CharType> & lstrip(std::basic_string<CharType> & s) {
    return lstrip(s, whitespaces<CharType>());
}

template< typename CharType >
std::basic_string<CharType> & strip(std::basic_string<CharType> & s, std::basic_string<CharType> const & strip_chars) {
    return lstrip(rstrip(s, strip_chars), strip_chars);
}

template< typename CharType >
std::basic_string<CharType> & strip(std::basic_string<CharType> & s) {
    return strip(s, whitespaces<CharType>());
}

template< typename CharType >
std::basic_string<CharType> lstrip_copy(std::basic_string<CharType> s, std::basic_string<CharType> const & strip_chars) {
    return lstrip(s, strip_chars);
}

template< typename CharType >
std::basic_string<CharType> rstrip_copy(std::basic_string<CharType> s, std::basic_string<CharType> const & strip_chars) {
    return rstrip(s, strip_chars);
}

template< typename CharType >
std::basic_string<CharType> strip_copy(std::basic_string<CharType> s, std::basic_string<CharType> const & strip_chars) {
    return strip(s, strip_chars);
}

template< typename CharType >
std::basic_string<CharType> lstrip_copy(std::basic_string<CharType> s) {
    return lstrip_copy(s, whitespaces<CharType>());
}

template< typename CharType >
std::basic_string<CharType> rstrip_copy(std::basic_string<CharType> s) {
    return rstrip_copy(s, whitespaces<CharType>());
}

template< typename CharType >
std::basic_string<CharType> strip_copy(std::basic_string<CharType> s) {
    return strip_copy(s, whitespaces<CharType>());
}

}}

#endif //GUARD_OGA_UTIL_STRIP_HPP_INCLUDED

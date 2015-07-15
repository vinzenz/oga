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

#ifndef GUARD_OGA_UTIL_ENCODING_HPP_INCLUDED
#define GUARD_OGA_UTIL_ENCODING_HPP_INCLUDED

#include <string>
#include <oga/base/types.hpp>

namespace oga {
namespace util {

typedef char utf8_char_t;
#if defined(_WIN32)
typedef wchar_t utf16_char_t;
#else
typedef uint16_t utf16_char_t;
#endif
typedef uint32_t utf32_char_t;

typedef std::basic_string<utf8_char_t>  utf8_string;
typedef std::basic_string<utf16_char_t> utf16_string;
typedef std::basic_string<utf32_char_t> utf32_string;

utf8_string utf16_to_utf8(utf16_char_t const * start, utf16_char_t const * end);
utf8_string utf32_to_utf8(utf32_char_t const * start, utf32_char_t const * end);

utf16_string utf8_to_utf16(utf8_char_t const * start, utf8_char_t const * end);
utf32_string utf8_to_utf32(utf8_char_t const * start, utf8_char_t const * end);

utf8_string utf16_to_utf8(utf16_char_t c);
utf8_string utf16_to_utf8(uint16_t c);

}}

#endif //GUARD_OGA_UTIL_ENCODING_HPP_INCLUDED


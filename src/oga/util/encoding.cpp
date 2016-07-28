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

#include <oga/util/enable_if.hpp>
#include <oga/util/encoding.hpp>
#include <oga/util/type_traits/is_same.hpp>
#include <utf8/checked.h>
#include <algorithm>

namespace oga {
namespace util {

utf8_string utf16_to_utf8(utf16_char_t const * start, utf16_char_t const * end)
{
    utf8_string result;
    utf8::utf16to8(
        reinterpret_cast<utf8::uint16_t const *>(start),
        reinterpret_cast<utf8::uint16_t const *>(end),
        std::back_inserter(result));
    return result;
}

utf8_string utf32_to_utf8(utf32_char_t const * start, utf32_char_t const * end)
{
    utf8_string result;
    utf8::utf32to8(
        reinterpret_cast<utf8::uint32_t const *>(start),
        reinterpret_cast<utf8::uint32_t const *>(end),
        std::back_inserter(result));
    return result;
}

utf16_string utf8_to_utf16(utf8_char_t const * start, utf8_char_t const * end)
{
    utf16_string result;
    utf8::utf8to16(
        reinterpret_cast<utf8::uint8_t const *>(start),
        reinterpret_cast<utf8::uint8_t const *>(end),
        std::back_inserter(result));
    return result;
}

utf32_string utf8_to_utf32(utf8_char_t const * start, utf8_char_t const * end)
{
    utf32_string result;
    utf8::utf8to32(
        reinterpret_cast<utf8::uint8_t const *>(start),
        reinterpret_cast<utf8::uint8_t const *>(end),
        std::back_inserter(result));
    return result;
}

utf8_string utf16_to_utf8(utf16_char_t c)
{
    utf16_char_t buf[] = {c, 0};
    return utf16_to_utf8(buf, buf + 2);
}

template< typename UInt16>
typename oga::util::enable_if<
    !oga::util::type_traits::is_same<UInt16, utf16_char_t>::value,
    utf8_string
>::type utf16_to_utf8(UInt16 c)
{
    utf16_char_t buf[] = {static_cast<utf16_char_t>(c), 0};
    return utf16_to_utf8(buf, buf + 2);
}

}}

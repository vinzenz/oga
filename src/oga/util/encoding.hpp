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

namespace oga {
namespace util {

typedef std::basic_string<uint8_t>  utf8_string;
typedef std::basic_string<uint16_t> utf16_string;
typedef std::basic_string<uint32_t> utf32_string;

utf8_string utf16_to_utf8(uint16_t const * start, uint16_t const * end);
utf8_string utf32_to_utf8(uint32_t const * start, uint32_t const * end);

utf16_string utf8_to_utf16(uint8_t const * start, uint8_t const * end);
utf16_string utf8_to_utf32(uint8_t const * start, uint8_t const * end);

utf8_string utf16_to_utf8(uint16_t c);

}}

#endif //GUARD_OGA_UTIL_ENCODING_HPP_INCLUDED


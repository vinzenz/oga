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

#ifndef GUARD_OGA_BASE_TYPES_HPP_INCLUDED
#define GUARD_OGA_BASE_TYPES_HPP_INCLUDED

#if defined(WIN32) || defined(WIN64)
#   include <windows.h>

typedef INT64 oga_int64_t;
typedef INT32 oga_int32_t;
typedef INT16 oga_int16_t;
typedef INT8  oga_int8_t;

typedef DWORD64 oga_uint64_t;
typedef DWORD32 oga_uint32_t;
typedef WORD	oga_uint16_t;
typedef BYTE	oga_uint8_t;

#else
#   include <stdint.h>

typedef int64_t oga_int64_t;
typedef int32_t oga_int32_t;
typedef int16_t oga_int16_t;
typedef int8_t  oga_int8_t;

typedef uint64_t oga_uint64_t;
typedef uint32_t oga_uint32_t;
typedef uint16_t oga_uint16_t;
typedef uint8_t  oga_uint8_t;

#endif

#if defined(__cplusplus)
namespace oga {
	typedef ::oga_int64_t int64_t;
	typedef ::oga_int32_t int32_t;
	typedef ::oga_int16_t int16_t;
	typedef ::oga_int8_t int8_t;

	typedef ::oga_uint64_t uint64_t;
	typedef ::oga_uint32_t uint32_t;
	typedef ::oga_uint16_t uint16_t;
	typedef ::oga_uint8_t uint8_t;
}
#endif

#endif //GUARD_OGA_BASE_TYPES_HPP_INCLUDED


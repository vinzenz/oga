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
#include <oga/base/thread.hpp>

#if defined(_WIN32)
#include <windows.h>

namespace oga {
namespace this_thread {
    inline NT_TIB * GetCurrentTIB() {
        return reinterpret_cast<NT_TIB*>(NtCurrentTeb());
    }

    thread_id id() {
        return static_cast<thread_id>(GetCurrentThreadId());
    }

    thread_handle handle() {
        return GetCurrentThread();
    }

    namespace {
        union addrconv_t {
            void * ptr;
            size_t addr;
        };

        size_t to_addr(void * ptr) {
            addrconv_t c;
            c.ptr = ptr;
            return c.addr;
        }

        template< typename T >
        T * to_ptr(size_t addr) {
            addrconv_t c;
            c.addr = addr;
            return reinterpret_cast<T *>(c.ptr);
        }
    }

    void name(char const * value) {
        if(GetCurrentTIB()->ArbitraryUserPointer == 0) {
            GetCurrentTIB()->ArbitraryUserPointer = const_cast<char*>(value);
        }
    }

    std::string name() {
        if(GetCurrentTIB()->ArbitraryUserPointer != 0) {
            return reinterpret_cast<char const *>(GetCurrentTIB()->ArbitraryUserPointer);
        }
        return std::string();
    }
}}

#endif

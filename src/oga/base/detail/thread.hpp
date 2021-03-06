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
#ifndef GUARD_OGA_BASE_DETAIL_THREAD_HPP_INCLUDED
#define GUARD_OGA_BASE_DETAIL_THREAD_HPP_INCLUDED

#include <oga/base/types.hpp>

#if defined(_WIN32)
#include <windows.h>


namespace oga {
    typedef HANDLE thread_handle;
    typedef uint64_t thread_id;
}
#else

#include <pthread.h>

namespace oga {
    typedef pthread_t thread_handle;
    typedef uint64_t  thread_id;
}
#endif

#endif //GUARD_OGA_BASE_DETAIL_THREAD_HPP_INCLUDED


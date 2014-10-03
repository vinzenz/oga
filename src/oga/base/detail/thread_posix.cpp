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

#if !defined(WIN32) && !defined(WIN64)

#include <unistd.h>
#include <sys/syscall.h>
#include <memory.h>

namespace oga {
namespace this_thread {

    thread_id id() {
        return static_cast<thread_id>(syscall(SYS_gettid));
    }

    thread_handle handle() {
        return pthread_self();
    }

    std::string name() {
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        pthread_getname_np(pthread_self(), buffer, sizeof(buffer));
        return buffer;
    }
}}

#endif

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
#ifndef GUARD_OGA_BASE_DETAIL_PROCESS_HPP_INCLUDED
#define GUARD_OGA_BASE_DETAIL_PROCESS_HPP_INCLUDED

#include <oga/base/errors.hpp>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <windows.h>


namespace oga {
    typedef HANDLE process_handle;
    typedef uint64_t process_id;
    typedef HANDLE process_stream_handle;


    struct popen_instance_state {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        HANDLE coninput;
        HANDLE conoutput;
        HANDLE conerror;

        popen_instance_state()
        : si()
        , pi()
        , coninput(INVALID_HANDLE_VALUE)
        , conoutput(INVALID_HANDLE_VALUE)
        , conerror(INVALID_HANDLE_VALUE)
        {
            si.dwFlags = 0;
            si.hStdOutput = INVALID_HANDLE_VALUE;
            si.hStdInput = INVALID_HANDLE_VALUE;
            si.hStdError = INVALID_HANDLE_VALUE;
        }

        ~popen_instance_state() {
            if(coninput != INVALID_HANDLE_VALUE) {
                ::CloseHandle(coninput);
            }
            if(conoutput != INVALID_HANDLE_VALUE) {
                ::CloseHandle(conoutput);
            }
            if(conerror != INVALID_HANDLE_VALUE) {
                ::CloseHandle(conerror);
            }
            ::CloseHandle(pi.hProcess);
            ::CloseHandle(pi.hThread);
        }
    };
}
#else
#include <unistd.h>
#include <sys/types.h>

namespace oga {
    typedef pid_t process_handle;
    typedef uint64_t  process_id;
    typedef int process_stream_handle;

    struct popen_instance_state {
        int conin;
        int conout;
        int conerr;
        int exit_code;
        int pid;

        ~popen_instance_state(){
            if(conin != -1 && conin != STDIN_FILENO)    ::close(conin);
            if(conout != -1 && conout != STDOUT_FILENO) ::close(conout);
            if(conerr != -1 && conerr != STDERR_FILENO) ::close(conerr);
        }
    };
}
#endif


namespace oga {
    enum popen_flags {
        kPopenNone          = 0x00,
        kPopenWrite         = 0x01,
        kPopenRead          = 0x02,
        kPopenReadWrite     = kPopenRead | kPopenWrite
    };

    namespace detail {
        struct process_info {
            std::vector<std::string> arguments;
            std::string working_directory;
            popen_flags flags;
            popen_instance_state state;

            process_info(
                    std::vector<std::string> const & arguments,
                    std::string const & working_directory,
                    popen_flags flags)
            : arguments(arguments)
            , working_directory(working_directory)
            , flags(flags)
            , state()
            {}
        };
    }
}
#endif //GUARD_OGA_BASE_DETAIL_PROCESS_HPP_INCLUDED

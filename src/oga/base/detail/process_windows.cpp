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
#include <oga/base/process.hpp>

#if defined(_WIN32)

namespace {
    struct CloseHandleGuard {
        CloseHandleGuard(HANDLE h)
        : handle_(h){}

        ~CloseHandleGuard() {
            if(handle_ != INVALID_HANDLE_VALUE && handle_ != NULL) {
                CloseHandle(handle_);
            }
        }
    protected:
        HANDLE handle_;
    };
}

namespace {
    inline std::string escape_cmdline_item(std::string item) {
        std::string result;
        result.reserve(item.size());
        bool contains_space = false;
        for(size_t i = 0; i < item.size(); ++i) {
            char c = item[i];
            switch(c) {
            case '"':
            case '\\':
                result +=  '\\';
                break;
            case ' ':
                contains_space = true;
                break;
            default:
                break;
            }
            result += c;
        }
        if(!contains_space) {
            return result;
        }
        return "\"" + result + "\"";
    }



    inline std::string make_cmdline(std::vector<std::string> const & args) {
        std::string result = args.front();
        for(size_t i = 1; i < args.size(); ++i) {
            result += " " + escape_cmdline_item(args[i]);
        }
        return result;
    }

    inline oga::error_type create_pipe(HANDLE & wr, HANDLE & rd, bool is_input) {
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), 0, TRUE};
        if(!CreatePipe(&rd, &wr, &sa, 0)) {
            return oga::get_last_system_error();
        }

        HANDLE tmp = INVALID_HANDLE_VALUE;
        if(!DuplicateHandle(GetCurrentProcess(), is_input ? wr : rd, GetCurrentProcess(), &tmp, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            CloseHandle(rd);
            CloseHandle(wr);
            rd = INVALID_HANDLE_VALUE;
            wr = INVALID_HANDLE_VALUE;
            return oga::get_last_system_error();
        }
        CloseHandle(is_input ? wr : rd);
        (is_input ? wr : rd) = tmp;
        return oga::success();
    }
}

namespace oga {
namespace this_process {

    process_id id() {
        return static_cast<process_id>(::GetCurrentProcessId());
    }
} // oga::this_process

    popen::popen(
            std::vector<std::string> const & arguments,
            std::string const & working_directory,
            popen_flags flags /*= kPopenRead*/)
    : info_(arguments, working_directory, flags)
    {
        raise_on_failure(start());
    }

    popen::~popen()
    {}

    error_type popen::read_stdout(std::string & data) {
        return stdread(info_.state.conoutput, data);
    }

    error_type popen::read_stderr(std::string & data) {
        return stdread(info_.state.conerror, data);
    }

    error_type popen::stdread(process_stream_handle fd, std::string & data) {
        for(;;) {
            char buffer[1024] = {};
            DWORD bytes_read = 0;
            if(!::ReadFile(fd, buffer, DWORD(sizeof(buffer)), &bytes_read, 0)) {
                return get_last_system_error();
            }
            data.append(buffer, buffer + bytes_read);
            if(bytes_read != sizeof(buffer)) {
                break;
            }
        }
        return success();
    }

    error_type popen::write_stdin(std::string const & b) {
        size_t total = 0;
        for(;total < b.size();) {
            DWORD written = 0;
            if(!::WriteFile(info_.state.coninput, b.c_str() + total, DWORD(b.size() - total), &written, 0)) {
                return get_last_system_error();
            }
            total += written;
        }
        return success();
    }

    error_type popen::wait() {
        if(::WaitForSingleObject(info_.state.pi.hProcess, INFINITE) == WAIT_FAILED) {
            return get_last_system_error();
        }
        return success();
    }

    int popen::exit_code() {
        DWORD code = 0;
        if(!::GetExitCodeProcess(info_.state.pi.hProcess, &code)) {
            return -1;
        }
        return int(code);
    }

    int popen::pid() {
        return info_.state.pi.dwProcessId;
    }

    error_type popen::start() {
        if(info_.flags & kPopenReadWrite) {
            info_.state.si.dwFlags |= STARTF_USESTDHANDLES;
            if(info_.flags & kPopenRead) {
                error_type err = create_pipe(info_.state.si.hStdOutput, info_.state.conoutput, false);
                if(err.code() != kAppErrSuccess) {
                    return err;
                }
                err = create_pipe(info_.state.si.hStdError, info_.state.conerror, false);
                if(err.code() != kAppErrSuccess) {
                    return err;
                }
            }
            else {
                info_.state.si.hStdError =  GetStdHandle(STD_ERROR_HANDLE);
                info_.state.si.hStdOutput =  GetStdHandle(STD_OUTPUT_HANDLE);
            }
            if(info_.flags & kPopenWrite) {
                error_type err = create_pipe(info_.state.coninput, info_.state.si.hStdInput, true);
                if(err.code() != kAppErrSuccess) {
                    return err;
                }
            }
            else {
                info_.state.si.hStdInput =  GetStdHandle(STD_INPUT_HANDLE);
            }
        }
        CloseHandleGuard cinp(info_.flags & kPopenWrite ? info_.state.si.hStdInput : INVALID_HANDLE_VALUE),
                         coutp(info_.flags & kPopenRead ? info_.state.si.hStdOutput : INVALID_HANDLE_VALUE),
                         cerr(info_.flags & kPopenRead ? info_.state.si.hStdError : INVALID_HANDLE_VALUE);
        std::string cmdline = make_cmdline(info_.arguments);
        if(!CreateProcess(
            /*lpApplicationName*/       info_.arguments[0].c_str(),
            /*lpCommandLine*/           const_cast<char*>(cmdline.c_str()),
            /*lpProcessAttributes*/     0,      // -> Default
            /*lpThreadAttributes*/      0,      // -> Default
            /*bInheritHandles*/         (info_.flags & kPopenReadWrite) ? TRUE : FALSE,
            /*dwCreationFlags*/         0,       // -> Default
            /*lpEnvironment*/           0,       // -> Default (inherited from parent process)
            /*lpCurrentDirectory*/      info_.working_directory.empty() ? 0 : info_.working_directory.c_str(), // Default from parent if empty
            /*lpStartupInfo*/           &info_.state.si,
            /*lpProcessInformation*/    &info_.state.pi
            )) {
            return get_last_system_error();
        }
        printf("Started process with PID %d - %s\n", pid(), cmdline.c_str());
        return success();
    }
}

#endif


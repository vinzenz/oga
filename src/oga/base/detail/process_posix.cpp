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
#include <oga/base/process.hpp>
#include <oga/base/error_def.hpp>
#include <errno.h>

#if !defined(_WIN32)

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>

namespace oga {
namespace this_process {

    process_id id() {
        return static_cast<process_id>(::getpid());
    }
} // oga::this_process

    popen::popen(
        std::vector<std::string> const & arguments,
        std::string const & working_directory,
        popen_flags flags)
    : info_(arguments, working_directory, flags)
    {
        start();
    }

    popen::~popen()
    {}

    error_type popen::read_stdout(std::string & data) {
        return stdread(info_.state.conout, data);
    }

    error_type popen::read_stderr(std::string & data) {
        return stdread(info_.state.conerr, data);
    }

    error_type popen::stdread(process_stream_handle fd, std::string & data) {
        for(;;) {
            char buffer[1024] = {};
            ssize_t bytes_read = ::read(fd, buffer, sizeof(buffer));
            if(bytes_read == -1) {
                error_type err = get_last_system_error();
                if(err.code() != EINTR) {
                    return err;
                }
            }
            data.append(buffer, buffer + bytes_read);
            if(bytes_read != sizeof(buffer)) {
                break;
            }
        }
        return error_type();
    }

    error_type popen::write_stdin(std::string const & b) {
        ssize_t total = 0;
        for(;total < b.size();) {
            ssize_t written = ::write(info_.state.conin, b.c_str() + total, b.size() - total);
            if(written == -1) {
                error_type err = get_last_system_error();
                if(err.code() != EINTR) {
                    return err;
                }
            }
            total += written;
        }
        return error_type();
    }

    error_type popen::wait() {
        int ret = ::waitpid(info_.state.pid, &info_.state.exit_code, 0);
        if(ret == -1) {
            return get_last_system_error();
        }
        return error_type();
    }

    int popen::exit_code() {
        return info_.state.exit_code;
    }

    int popen::pid() {
        return info_.state.pid;
    }


    inline error_type prep_pipe(int * p) {
#if defined(HAVE_PIPE2)
        errno = 0;
        if(pipe2(p, O_CLOEXEC) == -1)
#endif
        {
            errno = 0;
            if(pipe(p) != -1) {
                fcntl(p[0], F_SETFD, FD_CLOEXEC);
                fcntl(p[1], F_SETFD, FD_CLOEXEC);
            }
            else {
                return get_last_system_error();
            }
        }
        return error_type();
    }

    inline void close_fd(int * p) {
        if(p && p[0] != -1) {
            ::close(p[0]);
        }
        if(p && p[1] != -1) {
            ::close(p[1]);
        }
    }

    inline void close_all_fds(int * a, int * b, int * c) {
        close_fd(a); close_fd(b); close_fd(c);
    }

    error_type popen::start() {
        if(info_.arguments.empty()) {
            return oga::app_error(oga::kAppErrInvalidNumberOfArguments);
        }

        int conin[2] = {-1, -1}, conout[2] = {-1, -1}, conerr[2] = {-1, -1}, com[2] = {-1, -1};
        error_type err = error_type();

        err = prep_pipe(com);
        if(err.code() != 0) {
            return err;
        }

        if((info_.flags & kPopenWrite) == kPopenWrite) {
            err = prep_pipe(conin);
        }
        if(err.code() == 0 && (info_.flags & kPopenRead) == kPopenRead) {
            err = prep_pipe(conout);
            if(err.code() == 0) {
                err = prep_pipe(conerr);
            }
        }
        if(err.code() != 0) {
            close_all_fds(conin, conout, conerr);
            return err;
        }

        errno = 0;
        pid_t pid = fork();
        if(pid < 0) {
            return get_last_system_error();
        }

        // Child
        if(pid == 0) {
            close(com[0]);
            if(conin[1] != -1) {
                ::close(conin[1]);
                ::dup2(conout[0], 0);
            }
            if(conout[0] != -1) {
                ::close(conout[0]);
                ::dup2(conout[1], 1);
            }
            if(conerr[0] != -1) {
                ::close(conerr[0]);
                ::dup2(conerr[1], 2);
            }

            signal( SIGPIPE, SIG_DFL );
            signal( SIGCHLD, SIG_DFL );

            errno = 0;
            // Run execve(filename, args, env)
            if(!info_.working_directory.empty()) {
                ::chdir(info_.working_directory.c_str());
            }

            errno = 0;
            std::vector<char const*> args(info_.arguments.size(), "");
            for(size_t i = 0; i < info_.arguments.size(); ++i) {
                args[i] = info_.arguments[i].c_str();
            }

            execve(args[0], const_cast<char **>(&args[0]), environ);
            int err = errno;
            write(com[1], &err, sizeof(err));
            _exit(1);
        }
        // Parent
        else {
            ::close(conin[0]);
            ::close(conout[1]);
            ::close(conerr[1]);
            ::close(com[1]);
            info_.state.conin = conin[1];
            info_.state.conout = conout[0];
            info_.state.conerr = conerr[0];

            int code = 0;
            read(com[0], &code, sizeof(code));
            if(code != 0) {
                info_.state.exit_code = code;
            }
            else {
                info_.state.pid = pid;
                info_.state.exit_code = 0;
            }
        }
        return error_type();
    }
}

#endif

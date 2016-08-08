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
#ifndef GUARD_OGA_BASE_PROCESS_HPP_INCLUDED
#define GUARD_OGA_BASE_PROCESS_HPP_INCLUDED

#include <oga/base/detail/process.hpp>

namespace oga {
    namespace this_process {
        process_id id();
    }

    class popen {
        detail::process_info info_;
    public:
        popen(
            std::vector<std::string> const & arguments,
            std::string const & working_directory,
            popen_flags flags = kPopenRead);

        virtual ~popen();

        error_type read_stdout(std::string & data);
        error_type read_stderr(std::string & data);
        error_type write_stdin(std::string const & b);

        error_type wait();
        int exit_code();
        int pid();
    private:
        error_type start();
        error_type stdread(process_stream_handle fd, std::string & data);
    };
}

#endif //GUARD_OGA_BASE_PROCESS_HPP_INCLUDED

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

#if !defined(WIN32) && !defined(WIN64)

#include <oga/comm/detail/connection_linux.hpp>
#include <oga/proto/json/json_generator.hpp>
#include <oga/proto/json/json_parser.hpp>
#include <oga/base/errors.hpp>
#include <oga/base/error_def.hpp>

#include <algorithm>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <string.h>
#include <stdio.h>

static const unsigned int UNIX_MAX_PATH = 108;

namespace oga {
namespace comm {

connection::implementation::implementation()
{}

connection::implementation::~implementation()
{
    close();
}

error_type connection::implementation::connect(connection_params const & params) {
    errno = 0;
    int handle = socket(PF_UNIX, SOCK_STREAM, 0);
    if(handle >= 0) {

        sockaddr_un address;
        memset(&address, 0, sizeof(address));
        address.sun_family = AF_UNIX;

        ::strncpy(address.sun_path, params.address.c_str(), UNIX_MAX_PATH);

        errno = 0;
        if(!::connect(handle, (sockaddr*)&address, sizeof(sockaddr_un))) {
            close();
            handle_ = handle;
        }
    }
    return get_last_system_error();
}

error_type connection::implementation::close() {
    errno = 0;
    int h = handle_;
    handle_ = -1;
    if(h >= 0 && !::close(h)) {
        return get_last_system_error();
    }
    return error_type();
}

error_type connection::implementation::read_next_buffer() {
    errno = 0;
    buffers_.push_back(buffer());
    buffer & data = buffers_.back();
    error_type result;
    while((data.size = read(handle_, data.data, sizeof(data.data))) == -1) {
        result = get_last_system_error();
        if(result.code() != EINTR) {
            break;
        }
    }
    return result;
}

error_type connection::implementation::get_line(std::string & line) {
    bool completed = false;
    while(!completed) {
        if(buffers_.empty()) {
            error_type result = read_next_buffer();
            if(result.code() != 0) {
                return result;
            }
        }
        while(!buffers_.empty()) {
            buffer & b = buffers_.front();
            char const * start = b.data + b.pos;
            char const * end = b.data + b.size;
            char const * p = std::find(start, end, '\n');
            if(p != end) {
                line.append(start, p);
                b.pos = p - b.data;
                ++b.pos;
                completed = true;
            }
            else {
                line.append(start, end);
                buffers_.pop_front();
            }
        }
    }
    return error_type();
}

error_type connection::implementation::receive(message_type & message) {
    error_type result;
    std::string line;
    while((result = get_line(line)).code() == 0 && line.empty()) {
        result = read_next_buffer();
        if(result.code() != 0) {
            return result;
        }
    }
    oga::proto::json::value value;
    if(!parse(line.c_str(), line.c_str() + line.size(), value)) {
        return app_error(kAppErrInvalidMessage, kESevWarning);
    }
    if(value.type() != oga::proto::json::vt_object) {
        return app_error(kAppErrInvalidMessageFormat, kESevWarning);
    }
    message = value.get_object();
    return result;
}

error_type connection::implementation::send(message_type & message) {
    std::string msgstr = oga::proto::json::generate(message) + '\n';
    errno = 0;
    int result = ::write(handle_, msgstr.c_str(), int(msgstr.size()));
    if(result == -1) {
        return get_last_system_error();
    }
    return error_type();
}

}}

#endif

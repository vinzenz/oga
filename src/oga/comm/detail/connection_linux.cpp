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

#if !defined(_WIN32)

#include <oga/comm/detail/connection_linux.hpp>
#include <oga/base/errors.hpp>
#include <oga/base/error_def.hpp>

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
    int handle = socket(PF_UNIX, SOCK_CLOEXEC|SOCK_STREAM, 0);
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
    return success();
}

error_type connection::implementation::read_buffer(void * buffer, size_t buffer_size, size_t & bytes_read) {
    error_type result;
    errno = 0;
    while((bytes_read = read(handle_, buffer, buffer_size)) == ~size_t(0)) {
        result = get_last_system_error();
        if(result.code() != EINTR) {
            break;
        }
    }
    return result;
}

error_type connection::implementation::write_buffer(void const * buffer, size_t size) {
    errno = 0;
    int result = ::write(handle_, buffer, static_cast<int>(size));
    if(result == -1) {
        return get_last_system_error();
    }
    return success();
}


}}

#endif

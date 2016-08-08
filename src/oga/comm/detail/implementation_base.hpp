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
#ifndef GUARD_OGA_COMM_DETAIL_CONNECTION_BASE_HPP_INCLUDED
#define GUARD_OGA_COMM_DETAIL_CONNECTION_BASE_HPP_INCLUDED

#include <oga/base/errors.hpp>
#include <oga/base/error_def.hpp>
#include <oga/comm/connection.hpp>
#include <string>
#include <deque>

namespace oga {
namespace comm {

class implementation_base {
public:
    virtual ~implementation_base();

    error_type receive(connection::message_type & message);
    error_type send(connection::message_type & message);

protected:
    virtual error_type read_buffer(void * buffer,
                                   size_t buffer_size,
                                   size_t & bytes_read) = 0;
    virtual error_type write_buffer(void const * buffer, size_t size) = 0;
    error_type read_next_buffer();

    error_type get_line(std::string & line);

protected:
    struct buffer {
        char    data[4096];
        size_t  size;
        size_t  pos;
    };

protected:

    int handle_;
    std::deque<buffer> buffers_;
};

}}

#endif //GUARD_OGA_COMM_DETAIL_CONNECTION_BASE_HPP_INCLUDED

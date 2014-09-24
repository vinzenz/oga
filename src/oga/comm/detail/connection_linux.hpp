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

#ifndef GUARD_OGA_COMM_DETAIL_CONNECTION_LINUX_HPP_INCLUDED
#define GUARD_OGA_COMM_DETAIL_CONNECTION_LINUX_HPP_INCLUDED

#include <oga/comm/connection.hpp>
#include <string>
#include <deque>
namespace oga {
namespace comm {

class connection::implementation
{
public:
    implementation();
    virtual ~implementation();

    error_type connect(connection_params const & params);
    error_type close();

    error_type receive(message_type & message);
    error_type send(message_type & message);
protected:
    error_type read_next_buffer();
    error_type get_line(std::string & line);

    struct buffer {
        char data[4096];
        int  size;
        int  pos;
    };

    int handle_;
    std::deque<buffer> buffers_;
};

}}
#endif //GUARD_OGA_COMM_DETAIL_CONNECTION_LINUX_HPP_INCLUDED


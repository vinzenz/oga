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

#ifndef GUARD_OGA_COMM_DETAIL_CONNECTION_WINDOWS_HPP_INCLUDED
#define GUARD_OGA_COMM_DETAIL_CONNECTION_WINDOWS_HPP_INCLUDED

#include <oga/comm/detail/implementation_base.hpp>
#include <windows.h>

namespace oga {
namespace comm {

class connection::implementation : public implementation_base
{
    HANDLE handle_;
    OVERLAPPED read_ovl_;
    OVERLAPPED write_ovl_;
public:
    implementation();
    virtual ~implementation();

    error_type connect(connection_params const & params);
    error_type close();
protected:
    error_type read_buffer(void * buffer, size_t buffer_size, size_t & bytes_read);
    error_type write_buffer(void const * buffer, size_t buffer_size);
};

}}
#endif //GUARD_OGA_COMM_DETAIL_CONNECTION_WINDOWS_HPP_INCLUDED


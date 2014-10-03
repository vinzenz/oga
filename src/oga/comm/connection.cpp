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

#include <oga/comm/connection.hpp>

#if defined(_WIN32)
#   include <oga/comm/detail/connection_windows.hpp>
#else
#   include <oga/comm/detail/connection_linux.hpp>
#endif

namespace oga {
namespace comm {

connection::connection()
: impl_(new implementation())
{}

connection::~connection()
{}

error_type connection::connect(connection_params const & params) {
    return impl_->connect(params);
}

error_type connection::close() {
    return impl_->close();
}

error_type connection::receive(message_type & message) {
    return impl_->receive(message);
}

error_type connection::send(message_type & message) {
    return impl_->send(message);
}

}}

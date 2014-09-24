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

#ifndef GUARD_OGA_COMM_CONNECTION_HPP_INCLUDED
#define GUARD_OGA_COMM_CONNECTION_HPP_INCLUDED

#include <oga/base/errors.hpp>
#include <oga/proto/json/json.hpp>

#include <string>

namespace oga {
namespace comm {

struct connection_params {
    std::string address;
};

class connection {
public:
    typedef oga::proto::json::object message_type;

    connection();
    virtual ~connection();

    error_type connect(connection_params const & params);
    error_type close();

    error_type receive(message_type & message);
    error_type send(message_type & message);
private:
    class implementation;
    util::shared_ptr<implementation> impl_;
};

}}

#endif //GUARD_OGA_COMM_CONNECTION_HPP_INCLUDED

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

#ifndef GUARD_OGA_CORE_LOGIC_HPP_INCLUDED
#define GUARD_OGA_CORE_LOGIC_HPP_INCLUDED

#include <oga/handlers/handler.hpp>

namespace oga {

struct message {
    std::string name;
    message_args const * args;
    message_channel const * channel;
};

struct logic {
    void dispatch(message const & m);

};


}

#endif //GUARD_OGA_CORE_LOGIC_HPP_INCLUDED

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

#ifndef GUARD_OGA_PROTO_CONFIG_CONFIG_PARSER_HPP_INCLUDED
#define GUARD_OGA_PROTO_CONFIG_CONFIG_PARSER_HPP_INCLUDED

#include <oga/base/errors.hpp>
#include <oga/proto/json/json.hpp>

namespace oga {
namespace proto {
namespace config {

typedef oga::proto::json::object object;

error_type parse(char const * file_path, object & result);

}}}

#endif //GUARD_OGA_PROTO_CONFIG_CONFIG_PARSER_HPP_INCLUDED

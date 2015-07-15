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

#ifndef GUARD_OGA_CORE_PROVIDERS_DETAIL_APPLICATIONS_HPP_INCLUDED
#define GUARD_OGA_CORE_PROVIDERS_DETAIL_APPLICATIONS_HPP_INCLUDED

#include <set>
#include <string>

#include <oga/proto/config/config_parser.hpp>
#include <oga/util/shared_ptr.hpp>

namespace oga {
namespace core {
namespace providers {
namespace detail {

class applications {
public:
    virtual ~applications(){}
    virtual bool is_available() const = 0;
    virtual bool source_modified() const = 0;
    virtual oga::error_type refresh(std::set<std::string> & apps) = 0;
    virtual oga::error_type configure(oga::proto::config::object const & cfg) = 0;
};
typedef oga::util::shared_ptr<applications> applications_ptr;

}}}}

#endif //GUARD_OGA_CORE_PROVIDERS_DETAIL_APPLICATIONS_HPP_INCLUDED

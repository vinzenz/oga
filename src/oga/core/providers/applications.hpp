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

#ifndef GUARD_OGA_CORE_PROVIDERS_APPLICATIONS_HPP_INCLUDED
#define GUARD_OGA_CORE_PROVIDERS_APPLICATIONS_HPP_INCLUDED

#include <oga/core/data_provider.hpp>
#include <oga/util/shared_ptr.hpp>

namespace oga {
namespace core {
namespace providers {

class applications : public data_provider {
public:
    applications();
    oga::error_type configure(oga::proto::config::object const & cfg);
    virtual oga::error_type refresh();
    oga::error_type get(oga::proto::json::object & result,
                        std::string const & name,
                        protocol_version version);

    virtual bool source_modified() const;
protected:
    struct data;
    oga::util::shared_ptr<data> data_;
};

}}}

#endif //GUARD_OGA_CORE_PROVIDERS_APPLICATIONS_HPP_INCLUDED

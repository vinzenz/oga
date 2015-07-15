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
#ifndef GUARD_OGA_CORE_PROVIDERS_DETAIL_APPLICATIONS_LINUX_HPP_INCLUDED
#define GUARD_OGA_CORE_PROVIDERS_DETAIL_APPLICATIONS_LINUX_HPP_INCLUDED

#include <oga/core/providers/detail/applications.hpp>
#include <oga/base/logging.hpp>
#include <oga/util/split.hpp>

namespace oga {
namespace core {
namespace providers {
namespace detail {

class applications_linux : public applications {
public:
    applications_linux(oga::log::logger_ptr logger)
    : logger_(logger)
    , impl_()
    , apps_()
    {}

    virtual bool is_available() const {
        return impl_.ptr() != 0;
    }

    virtual oga::error_type configure(oga::proto::config::object const & cfg) {
        if(cfg.is_object("general")) {
            oga::proto::config::object const &  general = cfg["general"].get_object();
            if(general.is_string("applications_list")) {
                apps_ = oga::util::split(general["applications_list"].get_string(), ' ');
            }
        }
        return success();
    }

public:
    struct impl { virtual ~impl(){} };
protected:
    oga::log::logger_ptr logger_;
    oga::util::shared_ptr<impl> impl_;
    std::vector<std::string> apps_;
};

}}}}

#endif //GUARD_OGA_CORE_PROVIDERS_DETAIL_APPLICATIONS_LINUX_HPP_INCLUDED

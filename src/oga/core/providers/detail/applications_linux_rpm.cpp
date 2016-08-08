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

#include <oga/core/providers/detail/applications_linux_rpm.hpp>

#if !defined(_WIN32)

#if defined(OGA_WITH_RPM)
#include <rpm/rpmts.h>
#include <rpm/rpmdb.h>
#include <rpm/header.h>
#endif

namespace oga {
namespace core {
namespace providers {
namespace detail {

applications_linux_rpm::applications_linux_rpm(oga::log::logger_ptr logger)
: applications_linux(logger)
{}

bool applications_linux_rpm::is_available() const {
#if defined(OGA_WITH_RPM)
    return true;
#else
    return false;
#endif
}

oga::error_type applications_linux_rpm::refresh(std::set<std::string> & result)
{
#if defined(OGA_WITH_RPM)
    rpmts ts = rpmtsCreate();
    for(size_t i = 0; i < apps_.size(); ++i) {
        rpmdbMatchIterator iter = rpmtsInitIterator(ts, RPMTAG_NAME, apps_[i].c_str(), 0);
        Header h;
        while((h = rpmdbNextIterator(iter)) != 0) {
            char const * version = headerGetString(h, RPMTAG_VERSION);
            if(version) {
                result.insert(apps_[i] + "-" + std::string(version));
            }
        }
        rpmdbFreeIterator(iter);
    }
#endif
    return success();
}

bool applications_linux_rpm::source_modified() const {
    return true;
}

}}}}

#endif

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

#if !defined(_WIN32)
#include <oga/core/providers/detail/applications_linux_apt.hpp>


#if defined(OGA_WITH_APT)
#include <oga/util/split.hpp>
#include <apt-pkg/cachefile.h>
#include <apt-pkg/init.h>
#include <apt-pkg/pkgcache.h>
#endif

namespace oga {
namespace core {
namespace providers {
namespace detail {

struct app_apt_impl : public applications_linux_apt::impl {
#if defined(OGA_WITH_APT)
    pkgCacheFile pkgs;
#endif
};

typedef oga::util::impl_getter<app_apt_impl> get;

applications_linux_apt::applications_linux_apt(oga::log::logger_ptr logger)
: applications_linux(logger)
{
#if defined(OGA_WITH_APT)
    if(!pkgInitConfig(*_config)) {
        OGA_LOG_WARN(logger_, "Failed to initialize APT configuration");
        return;
    }
    if(!pkgInitSystem(*_config, _system)) {
        OGA_LOG_WARN(logger_, "Failed to initialize APT system");
        return;
    }
    impl_.reset(new app_apt_impl());
    OpProgress progress;
    if(!get(impl_)->pkgs.Open(progress, false)) {
        OGA_LOG_WARN(logger_, "Failed to open APT package cache.");
        // Dropping implementation to avoid using APT in this case
        impl_.reset();
    }
#endif
}

oga::error_type applications_linux_apt::refresh(std::set<std::string> & result)
{
#if defined(OGA_WITH_APT)
    pkgCache* cache = get(impl_)->pkgs;
    if(cache) {
        for(size_t i = 0; i < apps_.size(); ++i) {
            pkgCache::PkgIterator iter = cache->FindPkg(apps_[i]);
            if(!iter.end()) {
                if(iter->CurrentState == pkgCache::State::Installed) {
                    char const * version = iter.CurrentVer().VerStr();
                    result.insert(apps_[i] + "-" + std::string(version ? version : ""));
                } else {
                    pkgCache::PrvIterator provides = iter.ProvidesList();
                    while(!provides.end()) {
                        if(!provides.ParentPkg().end() && provides.ParentPkg()->CurrentState == pkgCache::State::Installed) {
                            char const * version = provides.ParentPkg().CurrentVer().VerStr();
                            result.insert(apps_[i] + "-" + std::string(version ? version : ""));
                        }
                        ++provides;
                    }
                }
            }
        }
    }
#endif
    return success();
}

bool applications_linux_apt::source_modified() const {
    return true;
}



}}}}

#endif

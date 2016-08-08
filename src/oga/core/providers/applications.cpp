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

#include <oga/core/providers/applications.hpp>

#if defined(_WIN32)
#   include <oga/core/providers/detail/applications_windows.hpp>
#else
#   include <oga/core/providers/detail/applications_linux_rpm.hpp>
#   include <oga/core/providers/detail/applications_linux_apt.hpp>
#endif

namespace oga {
namespace core {
namespace providers {

struct applications::data {
    std::vector< detail::applications_ptr > trackers;
    std::set<std::string>           current;
    bool                            changed;

    data(oga::log::logger_ptr logger)
    : trackers()
    , current()
    , changed(false)
    {
        OGA_LOG_DEBUG(logger, "Loading available application providers...");
#if defined(_WIN32)
        detail::applications_ptr winapps = detail::applications_ptr(new detail::applications_windows(oga::log::get("Provider.Applications.Windows")));
        OGA_LOG_INFO(logger, "Windows Application provider available.");
        trackers.push_back(winapps);
#else
        detail::applications_ptr linux_rpm = detail::applications_ptr(new detail::applications_linux_rpm(oga::log::get("Provider.Applications.RPM")));
        detail::applications_ptr linux_apt = detail::applications_ptr(new detail::applications_linux_apt(oga::log::get("Provider.Applications.APT")));
        if(linux_rpm->is_available()) {
            OGA_LOG_INFO(logger, "RPM Application provider available.");
            trackers.push_back(linux_rpm);
        } else {
            OGA_LOG_INFO(logger, "RPM Application provider NOT available.");
        }
        if(linux_apt->is_available()) {
            OGA_LOG_INFO(logger, "APT Application provider available.");
            trackers.push_back(linux_apt);
        } else {
            OGA_LOG_INFO(logger, "APT Application provider NOT available.");
        }
#endif
        OGA_LOG_DEBUG(logger, "Loading available application providers done.");
    }
    virtual ~data()
    {}
};

applications::applications()
: data_provider("applications", oga::log::get("Provider.Applications"), uint64_t(120))
, data_(new applications::data(logger_))
{
    add_provides_range("applications", kProtocolVersionLegacy, kProtocolVersionLatest);
}

oga::error_type applications::configure(oga::proto::config::object const & cfg)
{
    OGA_LOG_DEBUG(logger_, "Applying application provider configuration");
    if(cfg.count("general") != 0 && cfg.is_object("general")) {
        oga::proto::config::object const & general = cfg["general"].get_object();
        refresh_interval_ = uint64_t(general["report_application_rate"].get_integer(120));
        OGA_LOG_DEBUG(logger_, "Refresh interval {0}");
    }
    for(size_t i = 0; i < data_->trackers.size(); ++i) {
        data_->trackers[i]->configure(cfg);
    }
    OGA_LOG_DEBUG(logger_, "Applying application provider configuration");
    return success();
}

oga::error_type applications::refresh()
{
    std::set<std::string> latest;
    for(size_t i = 0; i < data_->trackers.size(); ++i) {
        data_->trackers[i]->refresh(latest);
    }
    if(!data_->changed && !latest.empty() && latest != data_->current) {
        data_->changed = true;
    }
    return success();
}

oga::error_type applications::get(oga::proto::json::object & result,
                                  std::string const & name,
                                  protocol_version)
{
    bool full = name == "applications.full";
    if(full || name == "applications") {
        if(full || data_->changed) {
            result["applications"] = oga::proto::json::array();
            oga::proto::json::array arr(data_->current.begin(),
                                        data_->current.end());
            result["applications"].get_array().swap(arr);
            return success();
        } else {
            return app_error(kAppErrNoChanges, kESevInformation);
        }
    }
    return app_error(kAppErrRequestedDataNotProvided, kESevWarning);
}

bool applications::source_modified() const {
    for(size_t i = 0; i < data_->trackers.size(); ++i) {
        if(data_->trackers[i]->source_modified()) {
            return true;
        }
    }
    return false;
}

}}}

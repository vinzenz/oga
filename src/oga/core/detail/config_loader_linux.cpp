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

#include <oga/proto/config/config_parser.hpp>
#include <oga/core/detail/config_loader.hpp>

namespace oga {
namespace core {
namespace detail {

#ifndef OGA_CONFIG_FILE_PATH
#   define OGA_CONFIG_FILE_PATH "/etc/ovirt-guest-agent.conf"
#endif
#ifndef OGA_DEFAULT_CONFIG_FILE_PATH
#   define OGA_DEFAULT_CONFIG_FILE_PATH "/usr/share/ovirt-guest-agent/default.conf"
#endif
#ifndef OGA_DEFAULT_LOGGER_CONFIG_FILE_PATH
#   define OGA_DEFAULT_LOGGER_CONFIG_FILE_PATH "/usr/share/ovirt-guest-agent/default-logger.conf"
#endif

oga::proto::config::object load_application_config() {
    oga::proto::config::object default_logger_config, default_config, config;
    oga::proto::config::parse(OGA_DEFAULT_CONFIG_FILE_PATH, default_config);
    oga::proto::config::parse(OGA_DEFAULT_LOGGER_CONFIG_FILE_PATH, default_logger_config);
    oga::proto::config::parse(OGA_CONFIG_FILE_PATH, config);

    update(default_config, default_logger_config);
    update(default_config, config);
    return default_config;
}


}}}
 #endif

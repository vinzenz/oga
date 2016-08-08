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

#include <oga/core/agent.hpp>
#include <oga/core/detail/config_loader.hpp>

#ifndef OVIRT_AGENT_VERSION
#   define OVIRT_AGENT_VERSION "development"
#endif

namespace oga {
namespace core {

agent::agent()
: config_(detail::load_application_config())
, logger_()
, connection_()
, loop_(connection_)
{
    oga::log::configure(config_);
    logger_ = oga::log::get("agent");
    OGA_LOG_INFO(logger_, "ovirt-guest-agent started - Version: {0}") % OVIRT_AGENT_VERSION;	
}

agent::~agent()
{
    OGA_LOG_INFO(logger_, "ovirt-guest-agent ended");
}



}}

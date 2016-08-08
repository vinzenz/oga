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

#include <oga/core/data_provider.hpp>

namespace oga {
namespace core {

data_provider::data_provider(std::string const & name,
                             oga::log::logger_ptr logger,
                             uint64_t refresh_interval/*= ~uint64_t(0)*/)
: logger_(logger)
, refresh_interval_(refresh_interval)
, name_(name)
, provides_()
{}

data_provider::~data_provider()
{}

oga::error_type data_provider::configure(oga::proto::config::object const & /*cfg*/)
{
    return success();
}

bool data_provider::needs_refresh() const {
    return refresh_interval_ != ~uint64_t(0);
}

uint64_t data_provider::refresh_interval() const {
    return refresh_interval_;
}

oga::error_type data_provider::refresh()
{
    return success();
}

std::string const & data_provider::name() const
{
    return name_;
}

data_provider::provides_map_type const & data_provider::provides() const
{
    return provides_;
}

oga::error_type data_provider::get(oga::proto::json::object &,
                                   std::string const &,
                                   protocol_version)
{
    return app_error(kAppErrNotImplemented, kESevWarning);
}

void data_provider::add_provides(std::string const & name,
                                 protocol_version version)
{
    provides_[name].insert(version);
}

void data_provider::add_provides_range(std::string const & name,
                                       protocol_version least,
                                       protocol_version maximum)
{
    for(int i = static_cast<int>(least); i < maximum && i < kProtocolVersionCount; ++i) {
        provides_[name].insert(static_cast<protocol_version>(i));
    }
}

bool data_provider::source_modified() const {
    return true;
}

}}

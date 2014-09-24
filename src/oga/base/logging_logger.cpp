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

#include <oga/base/logging.hpp>

namespace oga {
namespace log {

logger::logger(std::string const & name /*= "root"*/,
               log::level level /*= kLevelInfo*/,
               appender_ptr root_appender /*= default_appender()*/)
: level_(level)
, name_(name)
, appender_(root_appender)
{}

logger::~logger()
{}

std::string const & logger::name() const {
    return name_;
}

log::level logger::level() const {
    return level_;
}

void logger::level(log::level l) {
    level_ = l;
}

appender_ptr logger::appender() const {
    return appender_;
}

error_type logger::log(log::data const & d) {
    return appender_->append(d);
}

}}

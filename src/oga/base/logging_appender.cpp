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

#include <oga/base/logging_appender.hpp>
#include <cassert>

namespace oga {
namespace log {

appender::appender(std::string const & name, formatter_ptr formatter)
: name_(name)
, next_()
, formatter_(formatter)
{
}

std::string const & appender::name() const
{
    return name_;
}

appender::~appender()
{}

error_type appender::append(log::data const & d) {
    std::string message;
    if(formatter_) {
        error_type result = formatter_->apply(message, d);
        if(result.code() == kAppErrSuccess) {
            if(next()) {
                next()->write(message);
            }
            return write(message);
        }
    }
    if(next()) {
        next()->write(message);
    }
    return write(d.message);
}

appender_ptr appender::next() const {
    return next_;
}

void appender::push_next(appender_ptr n) {
    if(next_) {
        next_->push_next(n);
    }
    else {
        next_ = n;
    }
}

}}

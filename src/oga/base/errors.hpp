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

#ifndef GUARD_OGA_BASE_ERRORS_HPP_INCLUDED
#define GUARD_OGA_BASE_ERRORS_HPP_INCLUDED

#include <string>
#include <oga/base/types.hpp>

namespace oga {

enum error_severity {
    kESevSuccess     = 0x00000000,
    kESevInformation = 0x10000000,
    kESevWarning     = 0x80000000,
    kESevError       = 0xE0000000
};

class error_category {
public:
    virtual std::string message(int32_t code) const = 0;
};

error_category const & app_error_category();
error_category const & sys_error_category();

class error_type {
public:
    error_type();

    error_type(
            error_category const & category,
            int32_t code,
            error_severity severity
    );

    int32_t code() const;
    error_category const & category() const;
    error_severity severity() const;
    std::string message() const;
protected:
    error_category const * category_;
    int32_t code_;
    error_severity severity_;
};

error_type get_last_system_error();

error_type app_error(int32_t code, error_severity sev = kESevError);
error_type sys_error(int32_t code, error_severity sev = kESevError);

}

#endif //GUARD_OGA_BASE_ERRORS_HPP_INCLUDED


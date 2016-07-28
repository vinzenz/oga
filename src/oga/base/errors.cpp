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

#include <oga/base/errors.hpp>
#include <oga/base/error_def.hpp>

#if defined(WIN32) || defined(WIN64)
#   include <windows.h>
#else
#   include <errno.h>
#   include <string.h>
#endif

namespace oga {
class system_error_category : public error_category {
public:
    std::string message(int32_t code) const {
#if defined(WIN32) || defined(WIN64)
        std::string result;
        LPSTR text = 0;
        DWORD err = ::FormatMessageA(
                  FORMAT_MESSAGE_FROM_SYSTEM
                | FORMAT_MESSAGE_ALLOCATE_BUFFER
                | FORMAT_MESSAGE_IGNORE_INSERTS,
                0,
                DWORD(code),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&text,
                0);
        if(err == ERROR_SUCCESS && text != 0) {
            result.assign(text);
            ::LocalFree(text);
        }
        return result;
#else
        return strerror(code);
#endif
    }
};

error_category const & sys_error_category() {
    static system_error_category cat;
    return cat;
}

class application_error_category : public error_category {
public:
    std::string message(int32_t code) const {
        if(code < 0 || code >= kAppErr_COUNT) {
            return "Unknown error code";
        }
        return application_error_messages[code];
    }
};

error_category const & app_error_category() {
    static application_error_category cat;
    return cat;
}

error_type get_last_system_error() {
#if defined(WIN32) || defined(WIN64)
    return sys_error(int32_t(GetLastError()), kESevError);
#else
    return sys_error(errno, kESevError);
#endif
}


error_type app_error(int32_t code, error_severity sev /*= kESevError*/) {
    return error_type(app_error_category(), code, sev);
}

error_type sys_error(int32_t code, error_severity sev /*= kESevError*/) {
    return error_type(sys_error_category(), code, sev);
}

error_type com_error(int32_t code) {
    return error_type(system_error_category(),
                      code & 0x0000FFFF,
                      code < 0
                        ? kESevError
                        : code != 0
                            ? kESevInformation
                            : kESevSuccess);

}

error_type success() {
    return error_type();
}

error_type::error_type()
: category_(&sys_error_category())
, code_(0)
, severity_(kESevSuccess)
{}

error_type::error_type(
        error_category const & category,
        int32_t code,
        error_severity severity
)
: category_(&category)
, code_(code)
, severity_(severity)
{}

int32_t error_type::code() const {
    return code_;
}

error_category const & error_type::category() const {
    return *category_;
}

error_severity error_type::severity() const {
    return severity_;
}

std::string error_type::message() const {
    return category().message(code());
}


void raise_oga_error(error_type const & e) {
    throw oga_error(e);
}

void raise_on_failure(error_type const & e) {
    if(e.code() != kAppErrSuccess) {
        raise_oga_error(e);
    }
}
}

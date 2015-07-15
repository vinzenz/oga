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

#include <oga/util/shared_ptr.hpp>
#include <oga/util/encoding.hpp>

#ifndef GUARD_OGA_UTIL_REGISTR_HPP_INCLUDED
#define GUARD_OGA_UTIL_REGISTR_HPP_INCLUDED

#if defined(_WIN32)

#include <oga/base/errors.hpp>

namespace oga {
namespace util {

struct private_reg_handle_t
{};

class registry_handle {
public:
    registry_handle();
    registry_handle(registry_handle const & other);
    virtual ~registry_handle();

    oga::error_type open(registry_handle const & base, std::string const & path);
    oga::error_type get_string_value(std::string const & name, std::string & value);
    void close();

    operator bool() const;
protected:
    registry_handle(private_reg_handle_t const *);
protected:
    oga::util::shared_ptr<private_reg_handle_t const> internal_;
};

extern registry_handle const kRegRootLocalMachine;
extern registry_handle const kRegRootCurrentUser;
extern registry_handle const kRegRootClassesRoot;
extern registry_handle const kRegRootCurrentConfig;
extern registry_handle const kRegRootCurrentUserLocalSettings;
extern registry_handle const kRegRootPerformanceData;
extern registry_handle const kRegRootPerformanceNLSText;
extern registry_handle const kRegRootPerformanceText;
extern registry_handle const kRegRootUsers;


}}
#endif


#endif //GUARD_OGA_UTIL_REGISTR_HPP_INCLUDED

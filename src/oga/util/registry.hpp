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

#include <vector>
#include <oga/base/errors.hpp>

namespace oga {
namespace util {

struct private_reg_handle_t
{};


enum registry_value_type {
    kRegValNone                     =  0,
    kRegValString                   =  1,
    kRegValExpandString             =  2,
    kRegValBinary                   =  3,
    kRegValDWordLE                  =  4,
    kRegValDWordBE                  =  5,
    kRegValQWordLE                  =  6,
    //kRegValQWordBE                  =  7,
    kRegValLink                     =  8,
    kRegValMultiString              =  9,
    kRegValResourceList             = 10,
    kRegValFullResourceDescriptor   = 11,
    kRegValRequirementsList         = 12,

    //
    kRegValKey                      = 254,
    kRegValUnknown                  = 255
};

class registry_handle;
class registry_enum_handler {
public:
    virtual bool operator()(registry_handle const & handle,
                            std::string const & name,
                            registry_value_type type) = 0;
};

class registry_handle {
public:
    registry_handle();
    registry_handle(registry_handle const & other);
    virtual ~registry_handle();

    void swap(registry_handle & other);
    registry_handle & operator=(registry_handle h);

    // Comparison and odering is only by handle
    bool operator==(registry_handle const & other) const;
    bool operator!=(registry_handle const & other) const;
    bool operator<(registry_handle const & other) const;
    bool operator<=(registry_handle const & other) const;
    bool operator>(registry_handle const & other) const;
    bool operator>=(registry_handle const & other) const;

    // Opens by default the registry for reading, if write
    // is required, KEY_WRITE should be used as a flag
    // If a view change is required add KEY_WOW64_32KEY or
    // KEY_WOW64_64KEY respectively
    // Flags are combined with binary or
    oga::error_type open(registry_handle const & base,
                         std::string const & path,
                         uint32_t additional_flags = 0);

    oga::error_type get_string_value(std::string const & name,
                                     std::string & value);

    // Enumerates the names of values
    oga::error_type enum_values(registry_enum_handler & handler);

    // Enumerates the names of values and inserts it, into the vector
    oga::error_type enum_values(std::vector<std::string> & value_names);

    // Enumerates the names of keys
    oga::error_type enum_keys(registry_enum_handler & handler);

    // Enumerates the names of keys and inserts it into the vector
    oga::error_type enum_keys(std::vector<std::string> & key_names);

    // Duplicates the handle and creates a complete new instance
    // of the class with it's own reference count
    oga::error_type clone(registry_handle & handle) const;

    // Does only release the reference,
    // closes only the last open instance
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

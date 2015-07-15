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

#include <oga/util/registry.hpp>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NO_MINMAX
#include <windows.h>
#include <winreg.h>
#ifndef HKEY_CURRENT_USER_LOCAL_SETTINGS
#   define HKEY_CURRENT_USER_LOCAL_SETTINGS (( HKEY ) (ULONG_PTR)((LONG)0x80000007) )
#endif


namespace oga {
namespace util {
namespace {
    union registry_force_cast {
        private_reg_handle_t const * handle;
        HKEY hkey;
        size_t size;
    };
    private_reg_handle_t const * from_hkey(HKEY h) {
        registry_force_cast caster;
        caster.hkey = h;
        return caster.handle;
    }

    void reg_closer(private_reg_handle_t* key) {
        registry_force_cast caster;
        caster.handle = key;
        if(key == 0 || (caster.size & size_t(0xFFFFFF00)) == size_t(0x80000000)) {
            return;
        }
        RegCloseKey(caster.hkey);
    }

    class special_registry_handle : public registry_handle {
    public:
        special_registry_handle(HKEY hkey)
        : registry_handle(from_hkey(hkey))
        {}
    };

    class handle_retriever : public registry_handle {
    public:
        handle_retriever(registry_handle const & inner)
        : registry_handle(inner)
        {}

        HKEY hkey() const {
            if(internal_) {
                registry_force_cast caster;
                caster.handle = internal_.ptr();
                return caster.hkey;
            }
            return HKEY_LOCAL_MACHINE;
        }
    };
}

registry_handle const kRegRootLocalMachine              = special_registry_handle(HKEY_LOCAL_MACHINE);
registry_handle const kRegRootCurrentUser               = special_registry_handle(HKEY_CURRENT_USER);
registry_handle const kRegRootClassesRoot               = special_registry_handle(HKEY_CLASSES_ROOT);
registry_handle const kRegRootCurrentConfig             = special_registry_handle(HKEY_CURRENT_CONFIG);
registry_handle const kRegRootCurrentUserLocalSettings  = special_registry_handle(HKEY_CURRENT_USER_LOCAL_SETTINGS);
registry_handle const kRegRootPerformanceData           = special_registry_handle(HKEY_PERFORMANCE_DATA);
registry_handle const kRegRootPerformanceNLSText        = special_registry_handle(HKEY_PERFORMANCE_NLSTEXT);
registry_handle const kRegRootPerformanceText           = special_registry_handle(HKEY_PERFORMANCE_TEXT);
registry_handle const kRegRootUsers                     = special_registry_handle(HKEY_USERS);


registry_handle::registry_handle(private_reg_handle_t const * handle)
: internal_(handle, reg_closer)
{}


registry_handle::registry_handle()
{}

registry_handle::registry_handle(registry_handle const & other)
: internal_(other.internal_)
{}

registry_handle::operator bool() const {
    return internal_.ptr() != 0;
}

oga::error_type registry_handle::open(registry_handle const & base, std::string const & path)
{
    HKEY opened_key = HKEY();
    oga::util::utf16_string u16path = utf8_to_utf16(path.c_str(), path.c_str() + path.size());
    LONG result = ::RegOpenKeyW(handle_retriever(base).hkey(), u16path.c_str(), &opened_key);
    if(result == ERROR_SUCCESS) {
        internal_.reset(from_hkey(opened_key), reg_closer);
        return success();
    }
    return sys_error(int32_t(result));
}

oga::error_type registry_handle::get_string_value(std::string const & name, std::string & value)
{
    if(!(*this)) {
        return app_error(kAppErrInvalidHandle);
    }

    oga::util::utf16_string u16name = utf8_to_utf16(name.c_str(), name.c_str() + name.size());
    DWORD type = 0;
    DWORD size = 0;
    LONG result = RegQueryValueExW(handle_retriever(*this).hkey(), u16name.c_str(), 0, &type, 0, &size);
    if(result == ERROR_SUCCESS) {
        if(type != REG_SZ) {
            return app_error(kAppErrUnexpectedType);
        }
        oga::util::utf16_string buffer;
        buffer.resize(size + 1);
        result = RegQueryValueExW(handle_retriever(*this).hkey(), u16name.c_str(), 0, &type, reinterpret_cast<LPBYTE>(&buffer[0]), &size);
        if(result == ERROR_SUCCESS) {
            oga::util::utf8_string utf8value = oga::util::utf16_to_utf8(buffer.c_str(), buffer.c_str() + buffer.size());
            value.assign(utf8value.begin(), utf8value.end());
            return success();
        }
    }
    return sys_error(int32_t(result));
}

void registry_handle::close()
{
    internal_.reset();
}


}}

#endif

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
#include <oga/base/logging.hpp>

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
    registry_value_type from_win_reg_type(DWORD type) {
        switch(type) {
        case REG_NONE:
            return kRegValNone;
        case REG_BINARY:
            return kRegValBinary;
        case REG_SZ:
            return kRegValString;
        case REG_EXPAND_SZ:
            return kRegValExpandString;
        case REG_MULTI_SZ:
            return kRegValMultiString;
        case REG_DWORD_LITTLE_ENDIAN:
            return kRegValDWordLE;
        case REG_DWORD_BIG_ENDIAN:
            return kRegValDWordBE;
        case REG_QWORD_LITTLE_ENDIAN:
            return kRegValQWordLE;
        case REG_LINK:
            return kRegValLink;
        case REG_RESOURCE_REQUIREMENTS_LIST:
            return kRegValRequirementsList;
        case REG_FULL_RESOURCE_DESCRIPTOR:
            return kRegValFullResourceDescriptor;
        case REG_RESOURCE_LIST:
            return kRegValResourceList;
        default:
            return kRegValUnknown;
        }
        return kRegValNone;
    }

    DWORD to_win_reg_type(registry_value_type type) {
        switch(type) {
        case kRegValString:
            return REG_SZ;
        case kRegValExpandString:
            return REG_EXPAND_SZ;
        case kRegValBinary:
            return REG_BINARY;
        case kRegValDWordLE:
            return REG_DWORD_LITTLE_ENDIAN;
        case kRegValDWordBE:
            return REG_DWORD_BIG_ENDIAN;
        case kRegValQWordLE:
            return REG_QWORD_LITTLE_ENDIAN;
        case kRegValLink:
            return REG_LINK;
        case kRegValMultiString:
            return REG_MULTI_SZ;
        case kRegValResourceList:
            return REG_RESOURCE_LIST;
        case kRegValFullResourceDescriptor:
            return REG_FULL_RESOURCE_DESCRIPTOR;
        case kRegValRequirementsList:
            return REG_RESOURCE_REQUIREMENTS_LIST;
        case kRegValNone:
            return REG_NONE;
        default:
            break;
        }
        return REG_NONE;
    }


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

    HKEY hkey(registry_handle const & h) {
        return handle_retriever(h).hkey();
    }

    HKEY hkey(registry_handle const * h) {
        return handle_retriever(*h).hkey();
    }

    class registry_enum_name_collector : public registry_enum_handler {
    public:
        registry_enum_name_collector(std::vector<std::string> & target)
        : target_(target)
        {}
        bool operator()(registry_handle const &,
                        std::string const & name,
                        registry_value_type)
        {
            target_.push_back(name);
            return true;
        }
    private:
        std::vector<std::string> & target_;
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

registry_handle::~registry_handle()
{}

void registry_handle::swap(registry_handle & other)
{
    internal_.swap(other.internal_);
}

registry_handle & registry_handle::operator=(registry_handle h)
{
    swap(h);
    return *this;
}

bool registry_handle::operator==(registry_handle const & other) const
{
    return other.internal_.ptr() == internal_.ptr();
}

bool registry_handle::operator!=(registry_handle const & other) const
{
    return other.internal_.ptr() != internal_.ptr();
}

bool registry_handle::operator<(registry_handle const & other) const
{
    return internal_.ptr() < other.internal_.ptr();
}

bool registry_handle::operator>(registry_handle const & other) const
{
    return internal_.ptr() > other.internal_.ptr();
}

bool registry_handle::operator>=(registry_handle const & other) const
{
    return internal_.ptr() >= other.internal_.ptr();
}

bool registry_handle::operator<=(registry_handle const & other) const
{
    return internal_.ptr() <= other.internal_.ptr();
}

registry_handle::operator bool() const {
    return internal_.ptr() != 0;
}

oga::error_type registry_handle::open(registry_handle const & base,
                                      std::string const & path,
                                      uint32_t flags /*= 0*/)
{
    HKEY opened_key = HKEY();
    oga::util::utf16_string u16path = utf8_to_utf16(path.c_str(), path.c_str() + path.size());
    LONG result = ::RegOpenKeyExW(hkey(base), u16path.c_str(), 0, KEY_READ | flags, &opened_key);
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
    LONG result = RegQueryValueExW(hkey(this), u16name.c_str(), 0, &type, 0, &size);
    if(result == ERROR_SUCCESS) {
        if(type != REG_SZ) {
            return app_error(kAppErrUnexpectedType);
        }
        oga::util::utf16_string buffer;
        buffer.resize((size / sizeof(utf16_char_t)) + 1, 0);
        result = RegQueryValueExW(hkey(this), u16name.c_str(), 0, &type, reinterpret_cast<LPBYTE>(&buffer[0]), &size);
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

oga::error_type registry_handle::clone(registry_handle & handle) const
{
    HKEY new_hkey = 0;
    if(!DuplicateHandle(::GetCurrentProcess(), hkey(this),
                        ::GetCurrentProcess(), (LPHANDLE)&new_hkey,
                        0, FALSE, DUPLICATE_SAME_ACCESS)) {
        return get_last_system_error();
    }

    handle.internal_.reset(from_hkey(new_hkey), reg_closer);
    return success();
}


oga::error_type registry_handle::enum_values(std::vector<std::string> & value_names)
{
    registry_enum_name_collector collector(value_names);
    return enum_values(collector);
}

oga::error_type registry_handle::enum_keys(std::vector<std::string> & key_names)
{
    registry_enum_name_collector collector(key_names);
    return enum_keys(collector);
}

oga::error_type registry_handle::enum_values(registry_enum_handler & handler)
{
    DWORD number_of_values = 0;
    DWORD max_value_name_length = 0;
    HKEY key = hkey(this);
    LONG result = RegQueryInfoKeyW(key, 0, 0, 0, 0, 0, 0, &number_of_values, &max_value_name_length, 0, 0, 0);
    if(result == ERROR_SUCCESS) {
        max_value_name_length += 1; // Space for NULL termination
        oga::util::utf16_string buffer;
        for(DWORD i = 0; i < number_of_values; ++i) {
            buffer.clear();
            buffer.resize(max_value_name_length, 0);
            DWORD value_type = REG_NONE;
            DWORD len = max_value_name_length;
            result = RegEnumValueW(hkey(this), i, &buffer[0], &len, 0, &value_type, 0, 0);
            if(result == ERROR_SUCCESS) {
                if(!handler(*this,
                            oga::util::utf16_to_utf8(&buffer[0], &buffer[0] + wcslen(&buffer[0])),
                            from_win_reg_type(value_type))) {
                    break;
                }
            } else if(result == ERROR_NO_MORE_ITEMS) {
                break;
            } else if(result == ERROR_MORE_DATA) {
                max_value_name_length *= 2;
            } else {
                OGA_LOG_WARN(oga::log::get("oga.utils.registry"), "Failed to use RegEnumValueW - {0}") % result;
            }
        }
        return success();
    }
    return sys_error(result);
}

oga::error_type registry_handle::enum_keys(registry_enum_handler & handler)
{
    DWORD number_of_keys = 0;
    DWORD max_sub_key_length = 0;
    HKEY key = hkey(this);
    LONG result = RegQueryInfoKeyW(key, 0, 0, 0, &number_of_keys, &max_sub_key_length, 0, 0, 0, 0, 0, 0);
    if(result == ERROR_SUCCESS) {
        max_sub_key_length += 1; // Space for NULL termination
        oga::util::utf16_string buffer;
        for(DWORD i = 0; i < number_of_keys; ++i) {
            buffer.clear();
            buffer.resize(max_sub_key_length, 0);
            result = RegEnumKeyW(key, i, &buffer[0], max_sub_key_length);
            if(result == ERROR_SUCCESS) {
                if(!handler(*this,
                            oga::util::utf16_to_utf8(&buffer[0], &buffer[0] + wcslen(&buffer[0])),
                            kRegValKey)) {
                    break;
                }
            } else if(result == ERROR_NO_MORE_ITEMS) {
                break;
            } else if(result == ERROR_MORE_DATA) {
                max_sub_key_length *= 2;
            } else {
                OGA_LOG_WARN(oga::log::get("oga.utils.registry"), "Failed to use RegEnumKeyW - {0}") % result;
            }
        }
        return success();
    }
    return sys_error(result);
}

}}

#endif

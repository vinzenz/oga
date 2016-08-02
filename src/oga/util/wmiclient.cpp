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
#define _WIN32_DCOM
#include <oga/util/wmiclient.hpp>
#include <oga/util/encoding.hpp>

#if defined(_WIN32)
#include <map>
#include <comdef.h>
#include <wbemidl.h>

// void __stdcall _com_issue_error(HRESULT /*hr*/) {}

namespace _com_util {
    char * __stdcall ConvertBSTRToString(BSTR bstr) {
        const unsigned int stringLength = lstrlenW(bstr);
        oga::util::utf8_string u8 = oga::util::utf16_to_utf8(bstr, bstr + stringLength);
        char * ascii = new char [u8.size() + 1]();
        memcpy(ascii, u8.c_str(), u8.size());
        return ascii;
    }

    BSTR __stdcall ConvertStringToBSTR(char const * const a) {
        const size_t length = lstrlenA(a);
        oga::util::utf16_string u16 = oga::util::utf8_to_utf16(a, a + length);
        BSTR bstr = SysAllocStringLen(NULL, u16.size() + 1);
        memcpy(bstr, u16.c_str(), sizeof(oga::util::utf16_char_t) * u16.size());
        return bstr;
    }
}

namespace oga {
namespace util {

using _com_util::ConvertBSTRToString;
using _com_util::ConvertStringToBSTR;

std::string hex(int32_t code) {
    char buffer[11] = {};
    sprintf(buffer, "0x%.08X", code);
    return buffer;
}


wmi_client::wmi_client(oga::log::logger_ptr logger)
: logger_(logger)
, locator_()
, services_()
{
    ::CoInitializeEx(0, COINIT_MULTITHREADED);
    ::CoInitializeSecurity(0, -1, 0, 0, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, 0, EOAC_NONE, 0);
}

wmi_client::~wmi_client()
{
    disconnect();
    CoUninitialize();
}

oga::error_type wmi_client::connect()
{
    disconnect();
    HRESULT res = ::CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&locator_);
    if(FAILED(res)) {
        OGA_LOG_WARN(logger_, "Failed to create WbemLocator instance. Code: {0}") % hex(res);
        return com_error(res);
    }
    res = locator_->ConnectServer(_bstr_t(L"root\\CIMV2"), 0, 0, 0, 0, 0, 0, &services_);
    if(FAILED(res)) {
        OGA_LOG_WARN(logger_, "Failed to connect to server Code: {0}") % hex(res);
        return com_error(res);
    }
    return success();
}

oga::error_type wmi_client::disconnect()
{
    if(services_) services_.Release();
    if(locator_) locator_.Release();
    return success();
}

oga::proto::json::value variant_to_json_value(_variant_t const & v) {
    oga::proto::json::value result;
    switch(v.vt) {
    case VT_INT:
        result = oga::proto::json::to_value((INT)v);
        break;
    case VT_UINT:
        result = oga::proto::json::to_value((UINT)v);
        break;
    case VT_LPSTR:
	case VT_BSTR:
    case VT_LPWSTR:
        result = oga::proto::json::to_value(std::string(_bstr_t(v)));
        break;
    case VT_BOOL:
        result = oga::proto::json::to_value((BOOL)v);
        break;
    case VT_UI1:
        result = oga::proto::json::to_value((BYTE)v);
        break;
    case VT_UI2:
        result = oga::proto::json::to_value((uint16_t)v);
        break;
    case VT_UI4:
        result = oga::proto::json::to_value((uint32_t)v);
        break;
    case VT_UI8:
        result = oga::proto::json::to_value((uint64_t)v);
        break;
    case VT_I1:
        result = oga::proto::json::to_value((char)v);
        break;
    case VT_I2:
        result = oga::proto::json::to_value((int16_t)v);
        break;
    case VT_I4:
        result = oga::proto::json::to_value((int32_t)v);
        break;
    case VT_I8:
        result = oga::proto::json::to_value((int64_t)v);
        break;
    case VT_R4:
        result = oga::proto::json::to_value((float)v);
        break;
    case VT_R8:
    case VT_CY:
        result = oga::proto::json::to_value((double)v);
        break;
    default:
		OGA_LOG_WARN(oga::log::get("root"), "Unknown COM VariantType value type {0} - Do not know how to convert") % v.vt;
    case VT_EMPTY:
    case VT_NULL:
        break;
    }
    return result;
}

oga::error_type wmi_client::get_names(std::vector<std::string> & names,
			                          std::string const & table_name)
{

	return success();
}

oga::error_type wmi_client::query(oga::proto::json::array & result,
								  std::string const & q)
{
	_com_ptr_t<_com_IIID<IEnumWbemClassObject, &IID_IEnumWbemClassObject> > enumerator;
	HRESULT code = services_->ExecQuery(
		L"WQL",
		ConvertStringToBSTR(q.c_str()),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		0,
		&enumerator
		);
	if (FAILED(code)) {
		OGA_LOG_WARN(logger_, "Failed to execute query '{0}' with Code: {1}") % q % hex(code);
		return com_error(code);
	}
	std::vector<std::string> fields;
	static std::map<std::string, std::vector<std::string> > fields_map;

	_com_ptr_t<_com_IIID<IWbemClassObject, &IID_IWbemClassObject> > object;
	ULONG num_elements = 0;
	do {
		code = enumerator->Next(LONG(WBEM_INFINITE), 1, &object, &num_elements);
		if (FAILED(code)) {
			OGA_LOG_WARN(logger_, "Failed to get next element on query '{0}' with Code: {1}") % q % hex(code);
			break;
		}
		if (num_elements > 0) {
			oga::proto::json::object row;
			_variant_t class_name;
			code = object->Get(L"__CLASS", 0, &class_name, 0, 0);
			if (class_name.vt == VT_BSTR) {
				std::string tbl_name = (char const *)_bstr_t(class_name);
				if (fields_map.count(tbl_name) == 0 || fields_map.empty()) {
					SAFEARRAY * array = NULL;
					code = object->GetNames(NULL, WBEM_FLAG_LOCAL_ONLY, NULL, &array);
					if (FAILED(code)) {
						OGA_LOG_WARN(logger_, "WMI: Failed to get property names from enumerated object - query was '{0}' error was {1} - Result ignored") % q % hex(code);
						continue;
					}
					else {
						BSTR * name_list = 0;
						::SafeArrayAccessData(array, (LPVOID*)&name_list);
						LONG ubound = 0, lbound = 0;
						SafeArrayGetLBound(array, 1, &lbound);
						SafeArrayGetUBound(array, 1, &ubound);
						LONG count = ubound - lbound + 1;
						for (LONG i = 0; i < count; ++i) {
							fields.push_back((char const *)_bstr_t(name_list[i]));
						}

						::SafeArrayUnaccessData(array);
						::SafeArrayDestroy(array);
					}
					fields_map[tbl_name] = fields;
				}
			}

			for (size_t i = 0; i < fields.size(); ++i) {
				_variant_t v;
				oga::util::utf16_string name = oga::util::utf8_to_utf16(fields[i].c_str(), fields[i].c_str() + fields[i].size());
				code = object->Get(name.c_str(), 0, &v, 0, 0);
				if (FAILED(code)) {
					OGA_LOG_WARN(logger_, "Failed to get value for field '{0}' in query '{1}' Code: {2}") % fields[i] % q % hex(code);
				}
				else {
					row[fields[i]] = variant_to_json_value(v);
				}
			}
			result.push_back(row);
		}
	} while (num_elements > 0);
	return success();
}


oga::error_type wmi_client::query(oga::proto::json::array & result,
                                  std::vector<std::string> const & fields,
                                  std::string const & q)
{
	if (fields.empty()) return query(result, q);

    _com_ptr_t<_com_IIID<IEnumWbemClassObject, &IID_IEnumWbemClassObject> > enumerator;
    HRESULT code = services_->ExecQuery(
            L"WQL",
            ConvertStringToBSTR(q.c_str()),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            0,
            &enumerator
    );
    if(FAILED(code)) {
        OGA_LOG_WARN(logger_, "Failed to execute query '{0}' with Code: {1}") % q % hex(code);
        return com_error(code);
    }

    _com_ptr_t<_com_IIID<IWbemClassObject, &IID_IWbemClassObject> > object;
    ULONG num_elements = 0;
    do {
        code = enumerator->Next(LONG(WBEM_INFINITE), 1, &object, &num_elements);
        if(FAILED(code)) {
            OGA_LOG_WARN(logger_, "Failed to get next element on query '{0}' with Code: {1}") % q % hex(code);
            break;
        }
        if(num_elements > 0) {
            oga::proto::json::object row;

            for(size_t i = 0; i <  fields.size(); ++i) {
                _variant_t v;
				oga::util::utf16_string name = oga::util::utf8_to_utf16(fields[i].c_str(), fields[i].c_str() + fields[i].size());
                code = object->Get(name.c_str(), 0, &v, 0, 0);
                if(FAILED(code)) {
                    OGA_LOG_WARN(logger_, "Failed to get value for field '{0}' in query '{1}' Code: {2}") % fields[i] % q % hex(code);
                } else {
                    row[fields[i]] = variant_to_json_value(v);
                }
            }
            result.push_back(row);
        }
    } while(num_elements > 0);
    return success();
}

}}
#endif

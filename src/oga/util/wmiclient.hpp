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

#ifndef GUARD_OGA_UTIL_WMICLIENT_HPP_INCLUDED
#define GUARD_OGA_UTIL_WMICLIENT_HPP_INCLUDED

#if defined(_WIN32)

#include <oga/proto/json/json.hpp>
#include <oga/base/logging.hpp>
#include <comip.h>
#include <wbemcli.h>

namespace oga {
namespace util {

class wmi_client {
public:
    wmi_client(oga::log::logger_ptr logger);
    virtual ~wmi_client();

    oga::error_type connect();
	oga::error_type disconnect();

	oga::error_type query(oga::proto::json::array & result,
                          std::vector<std::string> const & fields,
                          std::string const & q);

	oga::error_type query(oga::proto::json::array & result,
						  std::string const & q);

	oga::error_type get_names(std::vector<std::string> & names,
							  std::string const & table_name);
protected:
    oga::log::logger_ptr logger_;
    _com_ptr_t<_com_IIID<IWbemLocator, &IID_IWbemLocator> > locator_;
    _com_ptr_t<_com_IIID<IWbemServices, &IID_IWbemServices> > services_;
};

}}

#endif

#endif //GUARD_OGA_UTIL_WMICLIENT_HPP_INCLUDED

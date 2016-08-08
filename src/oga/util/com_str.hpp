//
// Copyright 2014-2016 Vinzenz Feenstra, Red Hat, Inc. and/or its affiliates.
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

#ifndef GUARD_OGA_UTIL_COM_STR_HPP_INCLUDED
#define GUARD_OGA_UTIL_COM_STR_HPP_INCLUDED

#include <string>

#include <oga/util/encoding.hpp>

#if defined(_WIN32)

namespace oga {
namespace util {

class com_str {
public:
	com_str();
	explicit com_str(utf8_string const & s);
	explicit com_str(utf16_string const & s);
	explicit com_str(BSTR b);
	virtual ~com_str();

	utf8_string to_utf8() const;
	utf16_string to_utf16() const;
	
	BSTR get() const;
	BSTR * attach_to();
	void attach(BSTR str);
	BSTR detach();
	bool empty() const;
	void release();

protected:
	BSTR bstr_;


	com_str(com_str const &);
	com_str & operator=(com_str const &);
};

}}

#endif

#endif 
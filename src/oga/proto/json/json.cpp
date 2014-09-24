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

#include <oga/proto/json/json.hpp>


namespace oga {
namespace proto {
namespace json {

value::value(null/* = null()*/)
: holder_(new any_value<vt_null, null>(null()))
{}

value::value(std::string const & s)
: holder_(new any_value<vt_string, std::string>(s))
{}

value::value(double d)
: holder_(new any_value<vt_double, double>(d))
{}

value::value(int64_t i)
: holder_(new any_value<vt_integer, int64_t>(i))
{}

value::value(bool b)
: holder_(new any_value<vt_boolean, bool>(b))
{}

value::value(array const & a)
: holder_(new any_value<vt_array, array>(a))
{}

value::value(object const & o)
: holder_(new any_value<vt_object, object>(o))
{}

value::value(value const & value)
: holder_(value.holder_)
{}

value_type value::type() const {
    return holder_->type();
}

template< value_type VT, typename T>
T & get_v(util::shared_ptr<any_value_base> & p) {
    return static_cast<any_value<VT,T>*>(p.ptr())->value();
}

template< value_type VT, typename T, typename R>
R get_v(util::shared_ptr<any_value_base> const & p) {
    return static_cast<any_value<VT,T> const *>(p.ptr())->value();
}

std::string const & value::get_string() const {
    return get_v<vt_string, std::string, std::string const &>(holder_);
}

std::string & value::get_string() {
    return get_v<vt_string, std::string>(holder_);
}

double value::get_double() const {
    return get_v<vt_double, double, double>(holder_);
}

double & value::get_double() {
    return get_v<vt_double, double>(holder_);
}

int64_t value::get_integer() const {
    return get_v<vt_integer, int64_t, int64_t>(holder_);
}

int64_t & value::get_integer() {
    return get_v<vt_integer, int64_t>(holder_);
}

bool value::get_boolean() const {
    return get_v<vt_boolean, bool, bool>(holder_);
}
bool & value::get_boolean() {
    return get_v<vt_boolean, bool>(holder_);
}


array const & value::get_array() const {
    return get_v<vt_array, array, array const &>(holder_);
}

array & value::get_array() {
    return get_v<vt_array, array>(holder_);
}


object const & value::get_object() const {
    return get_v<vt_object, object, object const &>(holder_);
}

object & value::get_object(){
    return get_v<vt_object, object>(holder_);
}

double value::get_double(double def) const {
    if(type() == vt_double) {
        return get_double();
    }
    return def;
}

int64_t value::get_integer(int64_t def) const {
    if(type() == vt_integer) {
        return get_integer();
    }
    return def;
}

std::string const & value::get_string(std::string const & def) const {
    if(type() == vt_string) {
        return get_string();
    }
    return def;
}

bool value::get_boolean(bool def) const {
    if(type() == vt_boolean) {
        return get_boolean();
    }
    return def;
}

array const & value::get_array(array const & def) const {
    if(type() == vt_array) {
        return get_array();
    }
    return def;
}

object const & value::get_object(object const & obj) const {
    if(type() == vt_object) {
        return get_object();
    }
    return obj;
}

}}}


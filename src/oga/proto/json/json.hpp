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

#ifndef GUARD_OGA_PROTO_JSON_JSON_HPP_INCLUDED
#define GUARD_OGA_PROTO_JSON_JSON_HPP_INCLUDED

#include <stdint.h>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

#include <oga/util/shared_ptr.hpp>

namespace oga {
namespace proto {
namespace json {

    struct null{};
    struct object;
    struct array;

    enum value_type {
        vt_null = 0,
        vt_string = 1,
        vt_double = 2,
        vt_integer = 3,
        vt_boolean = 4,
        vt_array = 5,
        vt_object = 6
    };

    class any_value_base {
    public:
        virtual value_type type() const = 0;
        virtual ~any_value_base(){}
    };

    template<value_type ValueType, typename Type>
    class any_value : public any_value_base {
    public:
        any_value(Type const & v)
        : value_(v)
        {}

        Type & value() {
            return value_;
        }

        Type const & value() const {
            return value_;
        }

        value_type type() const {
            return ValueType;
        }
    protected:
        Type value_;
    };


    class value {
    public:
        value(null = null());
        value(std::string const & s);
        explicit value(double d);
        explicit value(int64_t i);
        explicit value(bool b);
        value(array const & a);
        value(object const & o);
        value(value const & value);

        value_type type() const;

        value & operator=(value v) {
            swap(v);
            return *this;
        }

        template< typename T >
        value & operator=(T const & t) {
            value v(t);
            swap(v);
            return *this;
        }

        void swap(value & rhs) {
            holder_.swap(rhs.holder_);
        }

        std::string const & get_string() const;
        std::string & get_string();

        // These are returning the default value
        // in case the type is not matching (e.g. vt_null)
        double get_double(double def) const;
        int64_t get_integer(int64_t def) const;
        bool get_boolean(bool def) const;
        std::string const & get_string(std::string const & def) const;
        array const & get_array(array const & def) const;
        object const & get_object(object const & obj) const;

        double get_double() const;
        double & get_double();

        int64_t get_integer() const;
        int64_t & get_integer();

        bool get_boolean() const;
        bool & get_boolean();

        array const & get_array() const;
        array & get_array();

        object const & get_object() const;
        object & get_object();

    protected:
        util::shared_ptr<any_value_base> holder_;
    };

    template< typename > struct tag;
    template<> struct tag<double> { typedef double type; };
    template<> struct tag<float> { typedef double type; };
    template<> struct tag<bool> { typedef bool type; };
	template<> struct tag<array> { typedef array type; };
	template<> struct tag<object> { typedef object type; };

    template< typename T >
    inline value to_value(T const & v, typename tag<T>::type* = 0) {
        return value(typename tag<T>::type(v));
    }
    inline value to_value(int64_t i) {
        return value(i);
    }
    inline value to_value(std::string const & i) {
        return value(i);
    }


    inline bool is_object(value const & v) {
        return v.type() == vt_object;
    }

    inline bool is_array(value const & v) {
        return v.type() == vt_array;
    }

    inline bool is_string(value const & v) {
        return v.type() == vt_string;
    }

    inline bool is_double(value const & v) {
        return v.type() == vt_double;
    }

    inline bool is_integer(value const & v) {
        return v.type() == vt_integer;
    }

    inline bool is_boolean(value const & v) {
        return v.type() == vt_boolean;
    }

    inline bool is_null(value const & v) {
        return v.type() == vt_null;
    }

    struct array : std::vector<value>
    {
        typedef std::vector<value> base_type;

        array()
        : base_type()
        {}

        template< typename ForwardIterator >
        array(ForwardIterator begin, ForwardIterator end)
        : base_type(begin, end)
        {}
    };

    struct object : std::map<std::string, value>
    {
        // using map::map;
        inline bool is(json::value_type type, std::string const & name) const {
            object::const_iterator iter = find(name);
            if(iter != end()) {
                return iter->second.type() == type;
            }
            return false;
        }

        inline bool is_null(std::string const & name) const {
            return is(vt_null, name);
        }
        inline bool is_integer(std::string const & name) const {
            return is(vt_integer, name);
        }
        inline bool is_double(std::string const & name) const {
            return is(vt_double, name);
        }
        inline bool is_boolean(std::string const & name) const {
            return is(vt_boolean, name);
        }
        inline bool is_string(std::string const & name) const {
            return is(vt_string, name);
        }
        inline bool is_array(std::string const & name) const {
            return is(vt_array, name);
        }
        inline bool is_object(std::string const & name) const {
            return is(vt_object, name);
        }

        inline value & operator[](std::string const & key) {
            std::map<std::string, value> & t = *this;
            return t[key];
        }
        inline value const & operator[](std::string const & key) const {
            const_iterator iter = lower_bound(key);
            if(iter == end()) {
                throw std::out_of_range(key + " is not a key of this json object.");
            }
            return iter->second;
        }
    };


    template< typename Visitable, typename Value >
    void apply_visitor(Visitable visitable, Value & v) {
        switch(v.type()) {
        case vt_null:
            visitable(null());
            break;
        case vt_double:
            visitable(v.get_double());
            break;
        case vt_string:
            visitable(v.get_string());
            break;
        case vt_integer:
            visitable(v.get_integer());
            break;
        case vt_boolean:
            visitable(v.get_boolean());
            break;
        case vt_array:
            visitable(v.get_array());
            break;
        case vt_object:
            visitable(v.get_object());
            break;
        default:
            break;
        }
    }


}}}

#endif //GUARD_OGA_PROTO_JSON_JSON_HPP_INCLUDED


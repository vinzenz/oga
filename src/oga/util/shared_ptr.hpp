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

#ifndef GUARD_OGA_UTIL_SHARED_PTR_HPP_INCLUDED
#define GUARD_OGA_UTIL_SHARED_PTR_HPP_INCLUDED

namespace oga {
namespace util {

template< typename T >
class shared_ptr {
    template<typename U>
    friend class shared_ptr;
public:
    explicit shared_ptr(T * t = 0)
    : p_(t)
    , r_(t != 0 ? new size_t(1) : 0)
    {
    }

    shared_ptr(shared_ptr const & rhs)
    : p_(rhs.p_)
    , r_(rhs.r_)
    {
        if(r_) ++(*r_);
    }

    template< typename U >
    shared_ptr(shared_ptr<U> const & rhs)
    : p_(static_cast<U* const>(rhs.p_))
    , r_(rhs.r_)
    {
        if(r_) ++(*r_);
    }

    virtual ~shared_ptr() {
        if(p_ && r_) {
            if(--(*r_) == 0) {
                delete p_;
                delete r_;
            }
            r_ = 0;
            p_ = 0;
        }
    }

    shared_ptr<T> & operator=(shared_ptr<T> rhs) {
        swap(rhs);
        return *this;
    }

    void swap(shared_ptr<T> & rhs) {
        std::swap(p_, rhs.p_);
        std::swap(r_, rhs.r_);
    }

    void reset(T * ptr = 0) {
        shared_ptr<T> tmp(ptr);
        swap(tmp);
    }

    operator bool() const {
        return ptr() != 0;
    }

    T const * ptr() const {
        return p_;
    }

    T * ptr() {
        return p_;
    }

    T const * operator->() const {
        return ptr();
    }

    T * operator->(){
        return ptr();
    }

    T & operator*() {
        return *ptr();
    }

    T const & operator*() const {
        return *ptr();
    }

protected:
    void unref() {
        if(p_ && r_) {
            if(--(*r_) == 0) {
                delete p_;
                delete r_;
                p_ = 0;
                r_ = 0;
            }
        }
    }
protected:
    T* p_;
    size_t* r_;
};

}}

#endif //GUARD_OGA_UTIL_SHARED_PTR_HPP_INCLUDED

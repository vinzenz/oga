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

#include <oga/base/types.hpp>
#include <algorithm>

namespace oga {
namespace util {


template< typename T >
void deleter(T * p) {
    delete p;
}

struct shared_ptr_aux {
	shared_ptr_aux(size_t count_) : count(count_) {}
	virtual ~shared_ptr_aux() {}

	size_t count;
	virtual void destroy() = 0;
};

template< typename T >
class shared_ptr {
    template<typename U>
    friend class shared_ptr;


	template< typename U, typename Closer>
	struct holder : shared_ptr_aux {
		Closer closer_;
		U * ptr_;

		holder(U * ptr, Closer closer, size_t count)
		: shared_ptr_aux(count)
		, closer_(closer)
		, ptr_(ptr) {
		}
		virtual void destroy() { closer_(ptr_); }
	};
public:
    template< typename CloserT >
    explicit shared_ptr(T * t, CloserT closer)
    : holder_(t != 0 ? new holder<T, CloserT>(t, closer, 1) : 0)
	, ptr_(t)
    {
    }


    explicit shared_ptr(T * t = 0)
    : holder_(t != 0 ? new holder<T, void(*)(T*)>(t, deleter<T>, 1) : 0)
	, ptr_(t)
    {
    }

    shared_ptr(shared_ptr const & rhs)
    : holder_(rhs.holder_)
	, ptr_(rhs.ptr_)
	{
        if(holder_) ++holder_->count;
    }

    template< typename U >
    shared_ptr(shared_ptr<U> rhs)
    : holder_(rhs.holder_)
    , ptr_(rhs.ptr_)
    {
		if (holder_) ++holder_->count;
    }

    virtual ~shared_ptr() {
        unref();
    }

    shared_ptr<T> & operator=(shared_ptr<T> rhs) {
        swap(rhs);
        return *this;
    }

    void swap(shared_ptr<T> & rhs) {
        std::swap(holder_, rhs.holder_);
		std::swap(ptr_, rhs.ptr_);
    }

    template< typename CloserT>
    void reset(T * ptr, CloserT closer) {
        shared_ptr<T> tmp(ptr, closer);
        swap(tmp);
    }

    void reset(T * ptr = 0) {
        shared_ptr<T> tmp(ptr);
        swap(tmp);
    }

    operator bool() const {
        return ptr() != 0;
    }

    T const * ptr() const {
        return ptr_;
    }

    T * ptr() {
        return ptr_;
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
        if(ptr_ && holder_) {
            if(--holder_->count == 0) {
                if(ptr_) {
					if (holder_) holder_->destroy();
                }
                delete holder_;
                ptr_ = 0;
                holder_ = 0;
            }
        }
    }
protected:
    T* ptr_;
	shared_ptr_aux *holder_;
};

template< typename T >
struct impl_getter {
    template< typename U >
    impl_getter(U * u)
    : p_(static_cast<T*>(u))
    {}

    template< typename U >
    impl_getter(shared_ptr<U> u)
    : p_(static_cast<T *>(u.ptr()))
    {}

    T const & operator*() const {
        return *p_;
    }

    T & operator*() {
        return *p_;
    }

    T * operator->() {
        return p_;
    }

    T const * operator->() const {
        return p_;
    }

private:
    T * p_;
};

}}


#endif //GUARD_OGA_UTIL_SHARED_PTR_HPP_INCLUDED

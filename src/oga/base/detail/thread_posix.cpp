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
#include <oga/base/thread.hpp>

#if !defined(WIN32) && !defined(WIN64)

#include <unistd.h>
#include <sys/syscall.h>
#include <memory.h>
#include <errno.h>
#include <stdio.h>
namespace oga {
    namespace this_thread {

        thread_id id() {
            return static_cast<thread_id>(syscall(SYS_gettid));
        }

        thread_handle handle() {
            return pthread_self();
        }

        std::string name() {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            pthread_getname_np(pthread_self(), buffer, sizeof(buffer));
            return buffer;
        }

        void name(char const * n) {
            if (n) {
                pthread_setname_np(pthread_self(), n);
            }
        }

        void sleep(uint64_t time_ms) {
            usleep(time_ms * 1000);
        }
    }

    void thread_runner(thread * t) {
        if(!t->name_.empty()) {
            this_thread::name(t->name_.c_str());
        }
        t->run();
    }

    static void* oga_thread_runner_posix(void * arg) {
        thread_runner(reinterpret_cast<thread*>(arg));
        return 0;
    }

    thread::thread(std::string const & name)
    : name_(name)
    , stop_(false)
    , handle_(0)
    {}

    thread::~thread()
    {
        stop(true);
    }

    void thread::start()
    {
        if (handle_ == 0) {
            if(::pthread_create(
                    &handle_,
                    0,
                    oga_thread_runner_posix,
                    static_cast<thread*>(this)
            ) == 0) {
                pthread_detach(handle_);
            }
        }
    }

    void thread::stop(bool should_wait)
    {
        if (handle_ != 0) {
            stop_ = true;
            if (should_wait) {
                wait();
            }
        }
    }

    error_type thread::wait(size_t const milliseconds) {
        if(handle_ == 0) {
            return success();
        }
        int ret = 0;
        if (milliseconds == ~size_t(0)) {
            ret = ::pthread_join(handle_, 0);
        }
        else {
            timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += milliseconds / 1000;
            ts.tv_nsec += (milliseconds % 1000) * 1000000;
            ret = ::pthread_timedjoin_np(handle_, 0, &ts);
            if (ret == ETIMEDOUT) {
                return sys_error(ETIMEDOUT);
            }
        }
        return oga::success();
    }

    thread_handle thread::handle() const {
        return handle_;
    }
    bool thread::should_stop() const {
        return stop_;
    }


    critical_section::critical_section()
    : lock_()
    {
        ::pthread_mutexattr_t attributes;
        ::pthread_mutexattr_init(&attributes);
        ::pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
        ::pthread_mutexattr_setprotocol(&attributes, PTHREAD_PRIO_INHERIT);
        ::pthread_mutex_init(&lock_, &attributes);
        ::pthread_mutexattr_destroy(&attributes);
    }

    critical_section::~critical_section() {
        ::pthread_mutex_destroy(&lock_);
    }

    bool critical_section::try_lock() const {
        return ::pthread_mutex_trylock(&lock_) == 0;
    }

    void critical_section::lock() const {
        ::pthread_mutex_lock(&lock_);
    }

    void critical_section::unlock() const {
        ::pthread_mutex_unlock(&lock_);
    }

    event::event(bool manual_reset)
    : signaled_(false)
    , manual_(manual_reset)
    , cond_()
    , lock_()
    {
        ::pthread_mutexattr_t attributes;
        ::pthread_mutexattr_init(&attributes);
        ::pthread_mutexattr_setprotocol(&attributes, PTHREAD_PRIO_INHERIT);
        ::pthread_mutex_init(&lock_, &attributes);
        ::pthread_mutexattr_destroy(&attributes);

        ::pthread_cond_init(&cond_, 0);
    }

    event::~event()
    {
        ::pthread_cond_destroy(&cond_);
        ::pthread_mutex_destroy(&lock_);
    }

    void event::set()
    {
        ::pthread_mutex_unlock(&lock_);
        if (!signaled_) {
            signaled_ = true;
            ::pthread_cond_broadcast(&cond_);
        }
        ::pthread_mutex_unlock(&lock_);
    }

    bool event::wait(int64_t time_millis)
    {
        ::pthread_mutex_unlock(&lock_);
        if (!signaled_)
        {
            if (time_millis >= 0) {
                timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += time_millis / 1000;
                ts.tv_nsec += (time_millis % 1000) * 1000000;
                do
                {
                    if (pthread_cond_timedwait(&cond_, &lock_, &ts) == ETIMEDOUT) {
                        ::pthread_mutex_unlock(&lock_);
                        return false;
                    }
                } while (!signaled_);
            }
            else {
                do
                {
                    ::pthread_cond_wait(&cond_, &lock_);
                } while (!signaled_);
            }

        }

        if (!manual_) {
            signaled_ = false;
        }

        ::pthread_mutex_unlock(&lock_);

        return true;
    }

    void event::reset()
    {
        ::pthread_mutex_lock(&lock_);
        signaled_ = false;
        ::pthread_mutex_unlock(&lock_);
    }
}

#endif

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
#include <oga/base/thread.hpp>

#if defined(_WIN32)
#include <windows.h>
#include <process.h>

namespace oga {
	namespace this_thread {
		inline NT_TIB * GetCurrentTIB() {
			return reinterpret_cast<NT_TIB*>(NtCurrentTeb());
		}

		thread_id id() {
			return static_cast<thread_id>(GetCurrentThreadId());
		}

		thread_handle handle() {
			return GetCurrentThread();
		}

		namespace {
			union addrconv_t {
				void * ptr;
				size_t addr;
			};

			size_t to_addr(void * ptr) {
				addrconv_t c;
				c.ptr = ptr;
				return c.addr;
			}

			template< typename T >
			T * to_ptr(size_t addr) {
				addrconv_t c;
				c.addr = addr;
				return reinterpret_cast<T *>(c.ptr);
			}
		}

		void name(char const * value) {
			if (GetCurrentTIB()->ArbitraryUserPointer == 0) {
				GetCurrentTIB()->ArbitraryUserPointer = const_cast<char*>(value);
			}
		}

		std::string name() {
			if (GetCurrentTIB()->ArbitraryUserPointer != 0) {
				return reinterpret_cast<char const *>(GetCurrentTIB()->ArbitraryUserPointer);
			}
			return std::string();
		}
	}

	void thread_runner(thread * t) {
		if (!t->name_.empty()) {
			this_thread::name(t->name_.c_str());
		}
		try {
			t->run();
		}
		catch (...) {

		}
		t->handle_ = INVALID_HANDLE_VALUE;
	}

	unsigned __stdcall thread_runner_win(void* t) {
		thread_runner(reinterpret_cast<thread*>(t));
		return 0;
	}

	thread::thread(std::string const & name)
		: name_(name)
		, stop_(false)
		, handle_(INVALID_HANDLE_VALUE)
	{
	}

	thread::~thread()
	{}

	void thread::start() {
		if (handle_ == INVALID_HANDLE_VALUE) {
			stop_ = false;
			handle_ = reinterpret_cast<thread_handle>(
				_beginthreadex(0, 0, thread_runner_win, this, 0, 0)
				);
		}
	}

	void thread::stop() {
		if (handle_ != INVALID_HANDLE_VALUE) {
			stop_ = false;
			wait();
		}
	}

	error_type thread::wait(size_t milliseconds) {
		if (handle_ == INVALID_HANDLE_VALUE) {
			return sys_error(ERROR_INVALID_HANDLE);
		}

		UINT t = INFINITE;
		if (milliseconds != ~size_t(0)) {
			t = UINT(milliseconds);
		}
		DWORD result = WaitForSingleObject(handle_, t);
		switch (result) {
		case WAIT_OBJECT_0:
			return success();
		case WAIT_ABANDONED:
			return sys_error(ERROR_ABANDONED_WAIT_0);
		case WAIT_TIMEOUT:
			return sys_error(ERROR_TIMEOUT, oga::kESevWarning);
		default:
			break;
		}
		return get_last_system_error();
	}

	critical_section::critical_section()
		: lock_() {
		::InitializeCriticalSection(&lock_);
	}

	critical_section::~critical_section() {
		::DeleteCriticalSection(&lock_);
	}

	bool critical_section::try_lock() const {
		return ::TryEnterCriticalSection(&lock_) == TRUE;
	}

	void critical_section::lock() const {
		EnterCriticalSection(&lock_);
	}

	void critical_section::unlock() const {
		LeaveCriticalSection(&lock_);
	}

	event::event(bool manual_reset)
	: handle_(::CreateEvent(0, manual_reset ? 1 : 0, 0, 0))
	{}

	event::~event()
	{
		::CloseHandle(handle_);
	}

	bool event::wait(int64_t time_millis)
	{
		if (handle_ != INVALID_HANDLE_VALUE) {
			DWORD time = time_millis == -1 ? INFINITE : DWORD(time_millis);
			DWORD result = WaitForSingleObject(handle_, time);
			switch (result) {
			case WAIT_OBJECT_0:
				return true;
			default:
				break;
			}
		} 
		return false;
	}

	void event::set()
	{
		if (handle_ != INVALID_HANDLE_VALUE) {
			::SetEvent(handle_);
		}
	}

	void event::reset()
	{
		if (handle_ != INVALID_HANDLE_VALUE) {
			::ResetEvent(handle_);
		}
	}
}

#endif

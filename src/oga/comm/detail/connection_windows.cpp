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
#if defined(_WIN32)
#include <oga/comm/detail/connection_windows.hpp>

namespace oga {
namespace comm {

connection::implementation::implementation()
: handle_(INVALID_HANDLE_VALUE)
, read_ovl_()
, write_ovl_()
{
    read_ovl_.hEvent = CreateEvent(0, TRUE, FALSE, 0);
    write_ovl_.hEvent = CreateEvent(0, TRUE, FALSE, 0);
}

connection::implementation::~implementation()
{
    close();
    ::CloseHandle(read_ovl_.hEvent);
    ::CloseHandle(write_ovl_.hEvent);
}

error_type connection::implementation::connect(connection_params const & params) {
    HANDLE handle = ::CreateFileA(
        params.address.c_str(),
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
    if(handle != INVALID_HANDLE_VALUE) {
        close();
        handle_ = handle;
    }
    else {
        return get_last_system_error();
    }
    return success();
}

error_type connection::implementation::close() {
    if(handle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
    }
    return success();
}

error_type connection::implementation::read_buffer(void * buffer, size_t buffer_size, size_t & bytes_read) {
    if(::ReadFile(handle_, buffer, static_cast<DWORD>(buffer_size), 0, &read_ovl_)) {
        DWORD read = 0;
        DWORD err = ::GetOverlappedResult(handle_, &read_ovl_, &read, TRUE);
        if(err != NO_ERROR) {
            return sys_error(err);
        }
        bytes_read = static_cast<size_t>(read);
    }
    else {
        return get_last_system_error();
    }
    return success();
}

error_type connection::implementation::write_buffer(void const * buffer, size_t buffer_size){
    if(::WriteFile(handle_, buffer, static_cast<DWORD>(buffer_size), 0, &write_ovl_)) {
        DWORD written = 0;
        DWORD err = ::GetOverlappedResult(handle_, &write_ovl_, &written, TRUE);
        if(err != NO_ERROR) {
            return sys_error(err);
        }
    }
    else {
        return get_last_system_error();
    }
    return success();
}

}}
#endif

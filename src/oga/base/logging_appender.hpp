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

#ifndef GUARD_OGA_BASE_LOGGING_APPENDER_HPP_INCLUDED
#define GUARD_OGA_BASE_LOGGING_APPENDER_HPP_INCLUDED

#include <oga/base/logging.hpp>
#include <sstream>
#include <stdio.h>

namespace oga {
namespace log {
namespace detail {


class null_appender : public appender {
public:
    null_appender(std::string const & name, formatter_ptr formatter, config const &)
    : appender(name, formatter)
    {}

    virtual error_type write(std::string const &) {
        return error_type();
    }
};

class console_appender : public appender {
public:
    console_appender(std::string const & name, formatter_ptr formatter, config const &)
    : appender(name, formatter)
    {}

protected:
    virtual error_type write(std::string const & message) {
        printf("%s\n", message.c_str());
        return error_type();
    }
};

class file_appender : public appender {
    FILE * handle_;
public:
    file_appender(std::string const & name, formatter_ptr formatter, config const & cfg)
    : appender(name, formatter)
    , handle_(0)
    {
        std::string const & filepath = cfg["path"].get_string();
        std::string const & openmode = cfg["mode"].get_string();
        handle_ = ::fopen(filepath.c_str(), openmode.c_str());
    }

    int64_t size() const {
        return ftell(handle_);
    }

    ~file_appender(){
        if(handle_) {
            ::fclose(handle_);
        }
    }

    virtual error_type write(std::string const &  message) {
        fwrite(message.c_str(), int(message.size()), 1, handle_);
        fputc('\n', handle_);
        return error_type();
    }
};

class rotating_file_appender : public appender {
    util::shared_ptr<file_appender> internal_;
    config config_;
    int64_t max_files_;
    int64_t max_size_;
public:
    rotating_file_appender(std::string const & name, formatter_ptr formatter, config const & cfg)
    : appender(name, formatter)
    , internal_(new file_appender(name, formatter_ptr(), cfg))
    , config_(cfg)
    {
        max_files_ = config_["count"].get_integer(5);
        max_size_ = config_["size"].get_integer(1 * kMiB);
    }

    virtual error_type write(std::string const & message) {
        if(internal_->size() + int64_t(message.size()) > max_size_) {
            error_type err = roll();
            if(err.code() != 0) {
                return err;
            }
        }
        return internal_->write(message);
    }

    error_type roll() {
        std::string path = config_["path"].get_string();
        std::ostringstream ostr;
        ostr <<  path << "." << max_files_;

        internal_.reset();
        ::remove(ostr.str().c_str());
        for(int64_t i = max_files_ - 1; i > 0; ++i) {
            std::ostringstream old_name;
            old_name << path << "." << i;
            std::ostringstream new_name;
            new_name << path << "." << i + 1;
            ::rename(old_name.str().c_str(), new_name.str().c_str());
        }
        internal_.reset(new file_appender(name(), formatter_ptr(), config_));
        return error_type();
    }
};

}}}

#endif //GUARD_OGA_BASE_LOGGING_APPENDER_HPP_INCLUDED

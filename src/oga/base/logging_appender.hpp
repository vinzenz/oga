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
#include <oga/util/strip.hpp>
#include <oga/util/split.hpp>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#if !defined(_WIN32)
#   include <syslog.h>
#endif

namespace oga {
namespace log {
namespace detail {


class null_appender : public appender {
public:
    null_appender(std::string const & name, formatter_ptr formatter, config const &)
    : appender(name, formatter)
    {}

    virtual error_type write(std::string const &) {
        return success();
    }
};

class syslog_appender : public appender {
public:
    syslog_appender(std::string const & name, formatter_ptr formatter, config const &)
    : appender(name, formatter)
    {
#if !defined(_WIN32)
        openlog("", LOG_PID, LOG_USER);
#endif
    }

    ~syslog_appender() {
    }

    virtual error_type write(std::string const & s) {
#if !defined(_WIN32)
        syslog(LOG_INFO, "%s", s.c_str());
#else
    (void)s;
#endif
        return success();
    }
protected:

};

class console_appender : public appender {
public:
    console_appender(std::string const & name, formatter_ptr formatter, config const &)
    : appender(name, formatter)
    {}
    virtual appender_ptr clone() const {
        return appender_ptr(new console_appender(name(), formatter_, config()));
    }

protected:
    virtual error_type write(std::string const & message) {
        printf("%s\n", message.c_str());
        return success();
    }
};

inline config convert_from_pycfg(config cfg) {
    using oga::util::strip;
    using oga::util::strip_copy;
    using oga::util::split;
    if(cfg.count("args")) {
        if(is_string(cfg["args"])) {
            std::vector<std::string> parts = split(strip(strip(strip(cfg["args"].get_string()), std::string("()"))), ',');
            if(parts.size() > 0)
                cfg["path"] = strip(parts[0], std::string(" '\""));
            if(parts.size() > 1)
                cfg["mode"] = strip(parts[1], std::string(" '\""));
            if(parts.size() > 2)
                cfg["size"] = int64_t(std::atoi(strip_copy(parts[2]).c_str()));
            if(parts.size() > 3)
                cfg["count"] = int64_t(std::atoi(strip_copy(parts[3]).c_str()));
            cfg.erase("args");
        }
    }
    return cfg;
}

class file_appender : public appender {
public:
    file_appender(std::string const & name, formatter_ptr formatter, config const & cfg)
    : appender(name, formatter)
    , handle_()
    {
        std::string const & filepath = cfg["path"].get_string();
        std::string openmode = cfg["mode"].get_string();
        // Special handling for not letting children inherit handles
#if defined(_MSC_VER)
        char cloexec_flag = 'N';
#else
        char cloexec_flag = 'e';
#endif
        if(openmode.find_first_not_of(cloexec_flag) == std::string::npos) {
            openmode += cloexec_flag;
        }
        handle_.reset(::fopen(filepath.c_str(), openmode.c_str()), ::fclose);
    }

    int64_t size() const {
        if(!handle_) return int64_t(0);
        FILE * f = const_cast<FILE*>(handle_.ptr());
        fseek(f, 0, SEEK_END);
        return ftell(f);
    }

    ~file_appender(){
    }

    virtual error_type write(std::string const &  message) {
        if(!handle_) return app_error(kAppErrInvalidHandle);
        fwrite(message.c_str(), int(message.size()), 1, handle_.ptr());
        fputc('\n', handle_.ptr());
        return success();
    }

protected:
    oga::util::shared_ptr<FILE> handle_;
};



class rotating_file_appender : public appender {
public:
    rotating_file_appender(std::string const & name, formatter_ptr formatter, config const & cfg)
    : appender(name, formatter)
    , config_(cfg)
    , internal_(new file_appender(name, formatter_ptr(), config_))
    {
        max_files_ = config_["count"].get_integer(5);
        max_size_ = config_["size"].get_integer(1 * kMiB);
    }

    virtual error_type write(std::string const & message) {
        int64_t size_cur = internal_->size();
        int64_t size_msg = int64_t(message.size());
        int64_t new_size = size_cur + size_msg;
        if(new_size > max_size_) {
            error_type err = roll();
            if(err.code() != kAppErrSuccess) {
                return err;
            }
        }
        return internal_->write(message);
    }

    error_type roll() {
        std::string path = config_["path"].get_string();
        std::ostringstream ostr;
        ostr <<  path << "." << max_files_ - 1;

        internal_.reset();
        ::remove(ostr.str().c_str());
        for(int64_t i = max_files_ - 2; i > -1; --i) {
            std::ostringstream old_name;
            old_name << path;
            if(i != 0) {
                old_name << "." << i;
            }
            std::ostringstream new_name;
            new_name << path << "." << i + 1;
            ::rename(old_name.str().c_str(), new_name.str().c_str());
        }
        internal_.reset(new file_appender(name(), formatter_ptr(), config_));
        return success();
    }

protected:
    config config_;
    util::shared_ptr<file_appender> internal_;
    int64_t max_files_;
    int64_t max_size_;
};

}}}

#endif //GUARD_OGA_BASE_LOGGING_APPENDER_HPP_INCLUDED

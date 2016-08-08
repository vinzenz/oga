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

#ifndef GUARD_OGA_BASE_LOGGING_HPP_INCLUDED
#define GUARD_OGA_BASE_LOGGING_HPP_INCLUDED

#include <oga/base/errors.hpp>
#include <oga/proto/json/json.hpp>
#include <oga/util/shared_ptr.hpp>
#include <oga/util/string_appender.hpp>
#include <algorithm>
#include <cstdio>

namespace oga {
namespace log {
    static int64_t const kKiB = 1024;
    static int64_t const kMiB = 1024 * kKiB;
    static int64_t const kGiB = 1024 * kMiB;
    static int64_t const kPiB = 1024 * kGiB;

    typedef oga::proto::json::object config;

    enum level {
        kLevelTrace      = 0x2000,
        kLevelDebug      = 0x1000,
        kLevelInfo       = 0x0800,
        kLevelWarning    = 0x0400,
        kLevelError      = 0x0200,
        kLevelFatal      = 0x0100,
        kLevelDisabled   = 0x0000
    };

    inline char const * levelname(log::level l) {
        switch(l) {
        case kLevelTrace:       return "TRACE";
        case kLevelDebug:       return "DEBUG";
        case kLevelInfo:        return "INFO";
        case kLevelWarning:     return "WARNING";
        case kLevelError:       return "ERROR";
        case kLevelFatal:       return "FATAL";
        case kLevelDisabled:    return "DISABLED";
        default:
            break;
        }
        return "UNKNOWN";
    }

    inline log::level conv_level(std::string l) {
        std::transform(l.begin(), l.end(), l.begin(), ::toupper);
        if(l == "TRACE") {
            return kLevelTrace;
        }
        else if(l == "DEBUG") {
            return kLevelDebug;
        }
        else if(l == "WARNING") {
            return kLevelWarning;
        }
        else if(l == "ERROR") {
            return kLevelError;
        }
        else if(l == "INFO") {
            return kLevelInfo;
        }
        else if(l == "FATAL") {
            return kLevelFatal;
        }
        else if(l == "DISABLED") {
            return kLevelDisabled;
        }
        return kLevelInfo;
    }

    struct data {
        log::level level;
        int line;
        char const * file;
        char const * func;
        std::string message;
        oga::proto::json::array values;
        char const * logger_name;
    };

    class appender;
    typedef oga::util::shared_ptr<appender> appender_ptr;

    appender_ptr default_appender();

    class logger;
    typedef oga::util::shared_ptr<logger> logger_ptr;
    class logger {
    public:
        logger(std::string const & name = "root",
               log::level level = kLevelInfo,
               appender_ptr appender = default_appender());
        virtual ~logger();

        virtual error_type          log(log::data const & d);
        virtual std::string const & name() const;
        virtual log::level          level() const;
        virtual void                level(log::level l);
        virtual appender_ptr        appender() const;
        virtual logger_ptr          derive(std::string const & name) const {
            return logger_ptr(new logger(name, level_, appender_));
        }

    protected:
        log::level level_;
        std::string const name_;
        appender_ptr appender_;
    };

    struct format_applicator_base {
        virtual ~format_applicator_base(){}
        virtual error_type apply(util::string_appender, log::data const &) const = 0;
    };
    typedef util::shared_ptr<format_applicator_base> format_applicator;

    class formatter {
    public:
        formatter(std::string const & name, std::string const & format);
        virtual ~formatter();

        virtual std::string const & name() const;
        virtual error_type          apply(std::string & result, log::data const & d) const;
        virtual error_type          set(std::string const & format_spec);
        virtual std::string         get() const;
    protected:
        std::string const name_;
        std::string format_;
        std::vector<format_applicator> applicators_;
    };
    typedef oga::util::shared_ptr<formatter> formatter_ptr;
    formatter_ptr default_formatter();

    class appender {
    public:
        appender(std::string const & name, formatter_ptr formatter);
        virtual ~appender();

        virtual std::string const & name() const;
        virtual error_type          append(log::data const &);
        virtual appender_ptr        next() const;
        virtual void                push_next(appender_ptr n);
    protected:
        virtual error_type          write(std::string const & message) = 0;

    protected:
        std::string const name_;
        appender_ptr next_;
        formatter_ptr formatter_;
    };

    struct log_performer {
        logger_ptr logger_;
        data data_;

        template< typename T >
        log_performer const & operator %(T const & t) const {
            const_cast<log_performer*>(this)->data_.values.push_back(oga::proto::json::to_value(t));
            return *this;
        }

        log_performer(logger_ptr logger, data const & d)
        : logger_(logger)
        , data_(d)
        {}

        ~log_performer() {
            try {
                if(logger_) {
                    logger_->log(data_);
                }
            } catch(...) {
            }
        }
    };

    logger_ptr get(char const * name);
    void configure(log::config const & configuration);

    inline data create_log_data(logger_ptr logger, log::level log_level, int line, char const * file, char const * func, std::string const & message) {
        data d;
        d.level = log_level;
        d.line = line;
        d.file = file;
        d.func = func;
        d.logger_name = logger->name().c_str();
        d.message = message;
        return d;
    }

#define OGA_LOG(LOGGER, SEVERITY, MESSAGE) oga::log::log_performer(LOGGER, oga::log::create_log_data(LOGGER, SEVERITY, __LINE__, __FILE__, __FUNCTION__, MESSAGE))

#define OGA_LOG_TRACE(LOGGER, MESSAGE) OGA_LOG(LOGGER, oga::log::kLevelTrace, MESSAGE)
#define OGA_LOG_DEBUG(LOGGER, MESSAGE) OGA_LOG(LOGGER, oga::log::kLevelDebug, MESSAGE)
#define OGA_LOG_INFO(LOGGER, MESSAGE) OGA_LOG(LOGGER, oga::log::kLevelInfo, MESSAGE)
#define OGA_LOG_WARN(LOGGER, MESSAGE) OGA_LOG(LOGGER, oga::log::kLevelWarning, MESSAGE)
#define OGA_LOG_ERROR(LOGGER, MESSAGE) OGA_LOG(LOGGER, oga::log::kLevelError, MESSAGE)
#define OGA_LOG_FATAL(LOGGER, MESSAGE) OGA_LOG(LOGGER, oga::log::kLevelFatal, MESSAGE)

}}

#endif //GUARD_OGA_BASE_LOGGING_HPP_INCLUDED

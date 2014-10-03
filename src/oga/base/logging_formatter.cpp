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

#include <oga/base/logging.hpp>
#include <oga/base/thread.hpp>
#include <oga/base/process.hpp>
#include <oga/util/split.hpp>
#include <sstream>
#include <time.h>
#include <memory.h>

namespace oga {
namespace log {

class function_applicator : public format_applicator_base {
public:
    template< typename F >
    function_applicator(F f)
    : f_(f)
    {}

    error_type apply(util::string_appender a, log::data const & d) const {
        return f_(a, d);
    }
protected:
    error_type (*f_)(util::string_appender, log::data const &);
};

class string_applicator : public format_applicator_base {
public:
    string_applicator(std::string const & s)
    : s_(s)
    {}

    error_type apply(util::string_appender a, log::data const &) const {
        a(s_);
        return error_type();
    }
protected:
    std::string s_;
};

namespace fmt {
    template< typename T >
    error_type streamout(util::string_appender a, T const & t) {
        std::ostringstream ostr;
        ostr << t;
        a(ostr.str());
        return error_type();
    }

    error_type message(util::string_appender a, log::data const & d) {
        a(d.message);
        return error_type();
    }

    error_type level(util::string_appender a, log::data const & d) {
        a(levelname(d.level));
        return error_type();
    }

    error_type line(util::string_appender a, log::data const & d) {
        return streamout(a, d.line);
    }

    error_type func(util::string_appender a, log::data const & d) {
        a(d.func ? d.func : "");
        return error_type();
    }

    error_type file(util::string_appender a, log::data const & d) {
        a(d.file ? d.file : "");
        return error_type();
    }

    error_type process(util::string_appender a, log::data const &) {
        return streamout(a, this_process::id());
    }

    error_type threadname(util::string_appender a, log::data const &) {
        a(this_thread::name());
        return error_type();
    }

    error_type thread(util::string_appender a, log::data const &) {
        return streamout(a, this_thread::id());
    }

    error_type timestamp(util::string_appender a, log::data const &) {
        time_t rawtime;
        struct tm * timeinfo = 0;
        time(&rawtime);
        timeinfo = gmtime(&rawtime);
        char buf[32];
        memset(buf, 0, sizeof(buf));
        if(strftime(buf, sizeof(buf) - 1, "%Y-%m-%dT%TZ", timeinfo) != 0) {
            a(buf);
        }
        return error_type();
    }

    error_type loggername(util::string_appender a, log::data const & d) {
        a(d.logger_name);
        return error_type();
    }
}

inline error_type parse(std::vector<format_applicator> & result,
                        std::string const & format_spec) {
    std::vector<std::string> a = util::split(format_spec, '%');
    for(size_t i = 0; i < a.size(); ++i) {
        if(!a[i].empty()) {
            bool formatter = false;
            if(a[i][0] == '(') {
                size_t pos = a[i].find_first_of(')');
                formatter = pos != std::string::npos;
                if(formatter) {
                    std::string name = a[i].substr(1, pos - 1);
                    if(a[i].size() > pos + 1) {
                        char x = a[i][pos + 1];
                        if(x == 'd' || x == 's') {
                            ++pos;
                        }
                    }
                    error_type (*fun)(util::string_appender, log::data const &) = 0;
                    if(name == "asctime") {
                        fun = fmt::timestamp;
                    }
                    else if(name == "threadName") {
                        fun = fmt::threadname;
                    }
                    else if(name == "tid") {
                        fun = fmt::thread;
                    }
                    else if(name == "pid") {
                        fun = fmt::process;
                    }
                    else if(name == "file" || name == "module") {
                        fun = fmt::file;
                    }
                    else if(name == "line" || name == "lineno") {
                        fun = fmt::line;
                    }
                    else if(name == "name") {
                        fun = fmt::loggername;
                    }
                    else if(name == "levelname") {
                        fun = fmt::level;
                    }
                    else if(name == "message") {
                        fun = fmt::message;
                    }

                    if(fun != 0) {
                        result.push_back(format_applicator(new function_applicator(fun)));
                        result.push_back(format_applicator(new string_applicator(a[i].substr(pos + 1))));
                    }
                    else {
                        formatter = false;
                    }
                }
            }
            if(!formatter) {
                result.push_back(format_applicator(new string_applicator((i == 0 ? "": "%") + a[i])));
            }
        }
    }

    return error_type();
}



formatter::formatter(std::string const & name, std::string const & format)
: name_(name)
, format_()
{
    if(parse(applicators_, format).code() != 0) {
        applicators_.push_back(format_applicator(new function_applicator(fmt::message)));
    }
}

formatter::~formatter()
{}

std::string const & formatter::name() const {
    return name_;
}

error_type formatter::apply(std::string & result, log::data const & d) const {
    std::string final;
    for(size_t i = 0; i < applicators_.size(); ++i) {
        error_type err = applicators_[i]->apply(util::string_appender(final), d);
        if(err.code() != 0) {
            return err;
        }
    }
    result.swap(final);
    return error_type();
}

error_type formatter::set(std::string const & format_spec) {
    std::vector<format_applicator> applicators;
    error_type err = parse(applicators, format_spec);
    if(err.code() == 0) {
        format_ = format_spec;
        applicators.swap(applicators_);
    }
    return err;
}

std::string formatter::get() const {
    return format_;
}

}}

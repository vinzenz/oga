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

#include <oga/base/logging_appender.hpp>
#include <oga/util/split.hpp>

namespace oga {
namespace log {

appender_ptr default_appender() {
    static appender_ptr ptr(new detail::null_appender("default", formatter_ptr(), config()));
    return ptr;
}

util::shared_ptr<logger> default_logger() {
    static util::shared_ptr<logger> ptr(new logger());
    return ptr;
}

struct repository {
    std::map<std::string, logger_ptr> loggers;
    std::map<std::string, appender_ptr> appenders;
    std::map<std::string, formatter_ptr> formatters;
};

void reset(repository & r) {
    r.loggers.clear();
    r.appenders.clear();
    r.formatters.clear();
}

repository & repo() {
    static repository r;
    return r;
}

logger_ptr get(char const * name) {
    if(repo().loggers.count(name) == 0) {
        return default_logger();
    }
    return repo().loggers[name];
}

void configure_appender(oga::proto::json::object const & configuration, std::string const & name) {
    std::string appender_name = "handler_" + name;
    config cfg = configuration[appender_name].get_object(config());
    formatter_ptr formatter = repo().formatters[cfg["formatter"].get_string("")];
    std::string cls = cfg["class"].get_string("default");
    if(cls == "rotating_file_appender") {
        repo().appenders[name].reset(new detail::rotating_file_appender(name, formatter, cfg));
    }
    else if(cls == "console_appender") {
        repo().appenders[name].reset(new detail::console_appender(name, formatter, cfg));
    }
    else if(cls == "file_appender") {
        repo().appenders[name].reset(new detail::file_appender(name, formatter, cfg));
    }
    else if(cls == "null_appender") {
        repo().appenders[name].reset(new detail::null_appender(name, formatter, cfg));
    }
}

void configure_formatter(oga::proto::json::object const & configuration, std::string const & name) {
    std::string formatter_name = "formatter_" + name;
    // TODO: Implement formatter configuration
    config cfg = configuration[formatter_name].get_object(config());
    repo().formatters[name].reset(new formatter(name, cfg["format"].get_string("%(message)s")));
}

void configure_logger(oga::proto::json::object const & configuration, std::string const & name) {
    std::string logger_name = "logger_" + name;
    // TODO: Implement logger configuration
    config cfg = configuration[logger_name].get_object(config());
    std::vector<std::string> appenders = util::split(cfg["handlers"].get_string(""), ',');
    appender_ptr appender = default_appender();
    for(size_t i = 0; i < appenders.size(); ++i) {
        appender_ptr a = repo().appenders[appenders[i]];
        if(a) {
            if(i == 0) {
                appender.swap(a);
            }
            else {
                appender->push_next(a);
            }
        }
    }
    repo().loggers[name].reset(new logger(name, kLevelInfo, appender));
}

void configure_loggers(oga::proto::json::object const & configuration, std::vector<std::string> const & names) {
    for(size_t i = 0; i < names.size(); ++i) {
        configure_logger(configuration, names[i]);
    }
}

void configure_appenders(oga::proto::json::object const & configuration, std::vector<std::string> const & names) {
    for(size_t i = 0; i < names.size(); ++i) {
        configure_appender(configuration, names[i]);
    }
}

void configure_formatters(oga::proto::json::object const & configuration, std::vector<std::string> const & names) {
    for(size_t i = 0; i < names.size(); ++i) {
        configure_formatter(configuration, names[i]);
    }
}

namespace {
    std::vector<std::string> get_keys(oga::proto::json::object const & configuration, std::string const & section_name) {
        config section = configuration[section_name].get_object(config());
        return util::split(section["keys"].get_string(""), ',');
    }
}

void configure(oga::proto::json::object const & configuration) {
    // The order should be formatters, appenders and then loggers as formatters are
    // used by appenders and appenders are used by loggers

    // 1. Configure formatters
    configure_formatters(configuration, get_keys(configuration, "formatters"));
    // 2. Configure appenders / handlers
    configure_appenders(configuration, get_keys(configuration, "handlers"));
    // 3. Configure loggers
    configure_loggers(configuration, get_keys(configuration, "loggers"));
}

}}

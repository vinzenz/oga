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

#include <oga/comm/connection.hpp>
#include <oga/proto/json/json_generator.hpp>
#include <oga/proto/json/json_parser.hpp>
#include <oga/proto/config/config_parser.hpp>
#include <oga/base/logging.hpp>
#include <oga/base/logging_appender.hpp>
#include <stdio.h>
#include <oga/base/process.hpp>
#include <fstream>
#include <utf8/checked.h>
#include <algorithm>
#include <oga/core/agent.hpp>
#include <oga/core/providers/detail/applications_linux_apt.hpp>
#include <oga/core/providers/detail/applications_linux_rpm.hpp>
#include <oga/core/providers/detail/applications_windows.hpp>
#include <oga/util/registry.hpp>
#include <oga/util/wmiclient.hpp>

static char const json_def[] = "{"
    "\"loggers\": {\"keys\": \"root\" },"
    "\"logger_root\": {\"level\": \"INFO\", \"handlers\": \"logfile\"},"
    "\"handlers\": {\"keys\": \"logfile\"},"
    "\"handler_logfile\": {\"class\": \"rotating_file_appender\", \"formatter\": \"long\", \"path\": \"/tmp/oga2.log\", \"mode\": \"a+\"},"
    "\"formatters\": {\"keys\": \"long\"},"
    "\"formatter_long\": {\"format\": \"[%(name)s]!%(levelname)s TID:%(tid)d PID:%(pid)d TS:%(asctime)s @%(module)s:%(lineno)d MSG:%(message)s\"}"
"}";

std::string filter(std::string inp) {
    std::vector<uint32_t> runes;
    utf8::utf8to32(inp.begin(), inp.end(), std::back_inserter(inp));
    for(size_t i = 0; i < runes.size(); ++i) {
        uint32_t & r = runes[i];
        if(r > utf8::internal::CODE_POINT_MAX
           || (r > 0xA && r < 0xD)
           || (r > 0xD && r < 0x20)
           || (r > 0x7E && r < 0x85)
           || (r > 0x85 && r < 0xA0)
           || r == 0xFFFE
           || r == 0xFFFF
           || !(utf8::internal::is_code_point_valid(r))) {
           r = 0xFFFD;
        }
    }
    inp.clear();
    utf8::utf32to8(runes.begin(), runes.end(), std::back_inserter(inp));
    return inp;
}

#if defined(_WIN32)
class printing_registry_enum_handler : public oga::util::registry_enum_handler {
public:
    bool operator()(oga::util::registry_handle const &,
                    std::string const & name,
                    oga::util::registry_value_type) {
        printf("Enum: %s\n", name.c_str());
        return true;
    }
};
#endif
int main(int argc, char const **argv) {
    oga::core::agent agent;

    OGA_LOG_DEBUG(oga::log::get("root"), "Hello World! {0} {1} {0} {1} '{2}' {3} {4} {5}") % "Some" % 3 % "arguments";
    OGA_LOG_INFO(oga::log::get("root"), "Hello World! {0} {1} '{2}' {3} {4} {5}") % "Some" % 3 % "arguments";
    OGA_LOG_ERROR(oga::log::get("root"), "Hello World! {0} {1} {0} {1} '{2}' {3} {4} {5}") % "Some" % 3 % "arguments";
#if defined(_WIN32)
    oga::core::providers::detail::applications_windows apt(oga::log::get("root"));
#else
    oga::core::providers::detail::applications_linux_apt apt(oga::log::get("root"));
#endif
#if defined(_WIN32)
    oga::util::registry_handle reg;
    oga::error_type reg_result = reg.open(oga::util::kRegRootLocalMachine, "System\\CurrentControlSet\\Services\\TermService");
    printing_registry_enum_handler reg_enum_printer;
    if(reg_result.code() == 0) {
        printf("Opening registry succeeded\nEnumerating value names:\n");
        reg.enum_values(reg_enum_printer);
        std::string value;
        reg_result = reg.get_string_value("ImagePath", value);
        printf("Getting string value result: %d - '%s' size: %u\n", reg_result.code(), value.c_str(), uint32_t(value.size()));
    }
    reg.open(oga::util::kRegRootLocalMachine, "System\\CurrentControlSet\\Services");
    printf("Enumerating services:\n");
    reg.enum_keys(reg_enum_printer);
#endif

    apt.configure(agent.config());
    std::set<std::string> apps_result;
    oga::error_type err = apt.refresh(apps_result);
    if(err.code() == 0) {
        printf("%u results\n", uint32_t(apps_result.size()));
    }
#if defined(_WIN32)
    oga::util::wmi_client wmi(oga::log::get("WMI"));
    printf("Pre-Connect\n");
    oga::error_type wmi_err = wmi.connect();
    if(wmi_err.code() == 0) {
        printf("Connected!\n");
        std::vector<std::string> fields;
        oga::proto::json::array wmi_result;
        wmi.query(wmi_result, oga::util::split(std::string(argv[1]), '|'), argv[2]);
    } else {
        OGA_LOG_ERROR(oga::log::get("root"), "Failed to connect to WMI provider: {0}") % wmi_err.code();
    }
#endif

    using namespace oga::proto::config;
    object cfg;
    oga::error_type result = parse("/home/vfeenstr/devel/work/ovirt/ovirt-guest-agent/configurations/default-logger.conf", cfg);
    if(result.code() == 0) {
        // cfg["handler_logfile"] = oga::log::detail::convert_from_pycfg(cfg["handler_logfile"].get_object());
        oga::log::configure(cfg);
        // printf("Generated: `%s`\n", generate(cfg).c_str());
    }

#if 0
    using namespace oga::proto::json;
    std::ifstream ifs("agentcapture-wxp.log");
    std::string line;
    while(std::getline(ifs, line)) {
        value res;
        if(parse_filter(line.data(), line.data() + line.size(), res, filter)) {
//            printf("%s\n", generate(res).c_str());
         // printf("Success\n");
        }
        else {
            printf("Failure\n");
        }
    }
#endif
#if 0
    using namespace oga::comm;
    oga::proto::json::value config;
    parse(json_def, json_def + (sizeof(json_def) - 1), config);
    oga::log::configure(config.get_object(oga::log::config()));

    oga::log::logger_ptr logger = oga::log::get("root");
    OGA_LOG_ERROR(logger, "Geez, what the hell?");

    connection_params params;
    params.address = "/tmp/test";
    connection con;
    oga::error_type err = con.connect(params);
    if(err.code() == kAppErrSuccess) {
        connection::message_type msg;
        err = con.receive(msg);
        printf("Receive result: %d - %s\n", err.code(), err.message().c_str());
        if(err.code() == kAppErrSuccess) {
            printf("Message: %s\n", generate(msg).c_str());
            printf("Name: %s - Address: %s\n", msg["name"].get_string().c_str(), msg["address"].get_string().c_str());
            con.send(msg);
        }
    }
    else {
        printf("Connect result: %d - %s\n", err.code(), err.message().c_str());
    }
#endif
#if 0
#if defined(WIN32)
    char const * args[] = {"C:\\windows\\system32\\cmd.exe", "/C", "echo foobar"};
#else
    char const *args[] = {"/bin/echo", "foobar"};
#endif
    std::vector<std::string> x(args, args + (sizeof(args) / sizeof(args[0])));
    try
    {
        printf("Output: ");
        fflush(stdout);
        oga::popen p(x, "", oga::kPopenNone);
        oga::raise_on_failure(p.wait());
        // oga::raise_on_failure(p.read_stdout(v));
    }
    catch(oga::oga_error const & e) {
        printf("Failed to start process: %d - %s\n", e.error().code(), e.what());
    }
    try
    {
        oga::popen p(x, "", oga::kPopenRead);
        std::string v;
        oga::raise_on_failure(p.read_stdout(v));
        oga::raise_on_failure(p.wait());
        printf("Output: %s", v.c_str());
    }
    catch(oga::oga_error const & e) {
        printf("Failed to start process: %d - %s\n", e.error().code(), e.what());
    }
#endif
}

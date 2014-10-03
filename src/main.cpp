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
#include <oga/base/logging.hpp>
#include <stdio.h>
#include <oga/base/process.hpp>

static char const json_def[] = "{"
    "\"loggers\": {\"keys\": \"root\" },"
    "\"logger_root\": {\"level\": \"INFO\", \"handlers\": \"logfile\"},"
    "\"handlers\": {\"keys\": \"logfile\"},"
    "\"handler_logfile\": {\"class\": \"rotating_file_appender\", \"formatter\": \"long\", \"path\": \"/tmp/oga2.log\", \"mode\": \"a+\"},"
    "\"formatters\": {\"keys\": \"long\"},"
    "\"formatter_long\": {\"format\": \"[%(name)s]!%(levelname)s TID:%(tid)d PID:%(pid)d TS:%(asctime)s @%(module)s:%(lineno)d MSG:%(message)s\"}"
"}";

int main() {
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
    if(err.code() == 0) {
        connection::message_type msg;
        err = con.receive(msg);
        printf("Receive result: %d - %s\n", err.code(), err.message().c_str());
        if(err.code() == 0) {
            printf("Message: %s\n", generate(msg).c_str());
            printf("Name: %s - Address: %s\n", msg["name"].get_string().c_str(), msg["address"].get_string().c_str());
            con.send(msg);
        }
    }
    else {
        printf("Connect result: %d - %s\n", err.code(), err.message().c_str());
    }
#endif
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

}

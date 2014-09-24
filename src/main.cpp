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

static char const json_def[] = "{"
    "\"loggers\": {\"keys\": \"root\" },"
    "\"logger_root\": {\"level\": \"INFO\", \"handlers\": \"logfile\"},"
    "\"handlers\": {\"keys\": \"logfile\"},"
    "\"handler_logfile\": {\"class\": \"rotating_file_appender\", \"formatter\": \"long\", \"path\": \"/tmp/oga2.log\", \"mode\": \"a+\"},"
    "\"formatters\": {\"keys\": \"long\"},"
    "\"formatter_long\": {\"format\": \"%(threadName)s::%(levelname)s::%(asctime)s::%(module)s::%(lineno)d::%(name)s::%(message)s\"}"
"}";

int main() {
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
}

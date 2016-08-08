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

#include <oga/proto/config/config_parser.hpp>
#include <oga/proto/json/json_parser.hpp>
#include <oga/util/strip.hpp>
#include <oga/util/split.hpp>
#include <string>
#include <fstream>

namespace oga {
namespace proto {
namespace config {

using oga::util::strip;
using oga::util::strip_copy;
using oga::util::split;

namespace {
    oga::proto::json::value convert(std::string const & v) {
        oga::proto::json::value result;
        if(oga::proto::json::parse(v.c_str(), v.c_str() + v.size(), result)) {
            return result;
        }
        return oga::proto::json::value(v);
    }

    void assign(object & target, std::string const & key, std::string const & value) {
        target[strip_copy(key)] = convert(value);
    }
}

error_type parse(char const * file_path, object & result) {
    std::ifstream ifs(file_path);
    if(ifs) {
        std::string line;
        std::string section;
        while(std::getline(ifs, line)) {
            strip(line);
            if(line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }

            if(line[0] == '[') {
                if(line.size() > 2) {
                    section = strip_copy(line.substr(1, line.size() - 2));
                    if(result.count(section) == 0) {
                        result[section] = object();
                    }
                }
            } else {
                char used_sep = '=';
                std::vector<std::string> parts = split(line, used_sep);
                if(parts.size() == 1) {
                    used_sep = ':';
                    parts = split(line, used_sep);
                }
                if(parts.size() == 1) {
                    result[section].get_object()[strip_copy(parts[0])] = oga::proto::json::null();
                } else if(parts.size() > 0){
                    assign(result[section].get_object(), parts[0], strip_copy(line.substr(parts[0].size() + 1)));
                }
            }
        }
    }
    return success();
}

}}}

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

#include <oga/comm/detail/implementation_base.hpp>
#include <oga/proto/json/json_parser.hpp>
#include <oga/proto/json/json_generator.hpp>

#include <algorithm>

namespace oga {
namespace comm {

implementation_base::~implementation_base()
{}

error_type implementation_base::read_next_buffer() {
    buffers_.push_back(buffer());
    buffer & data = buffers_.back();
    error_type result = read_buffer(data.data, sizeof(data.data), data.size);
    if(result.code() != 0) {
        buffers_.pop_back();
    }
    return result;
}

error_type implementation_base::get_line(std::string & line) {
    bool completed = false;
    while(!completed) {
        if(buffers_.empty()) {
            error_type result = read_next_buffer();
            if(result.code() != 0) {
                return result;
            }
        }
        while(!buffers_.empty()) {
            buffer & b = buffers_.front();
            char const * start = b.data + b.pos;
            char const * end = b.data + b.size;
            char const * p = std::find(start, end, '\n');
            if(p != end) {
                line.append(start, p);
                b.pos = p - b.data;
                ++b.pos;
                completed = true;
            }
            else {
                line.append(start, end);
                buffers_.pop_front();
            }
        }
    }
    return error_type();
}

error_type implementation_base::receive(connection::message_type & message) {
    error_type result;
    std::string line;
    while((result = get_line(line)).code() == 0 && line.empty()) {
        result = read_next_buffer();
        if(result.code() != 0) {
            return result;
        }
    }
    oga::proto::json::value value;
    if(!parse(line.c_str(), line.c_str() + line.size(), value)) {
        return app_error(kAppErrInvalidMessage, kESevWarning);
    }
    if(value.type() != oga::proto::json::vt_object) {
        return app_error(kAppErrInvalidMessageFormat, kESevWarning);
    }
    message = value.get_object();
    return result;
}

error_type implementation_base::send(connection::message_type & message) {
    std::string msgstr = oga::proto::json::generate(message) + '\n';
    return write_buffer(msgstr.c_str(), msgstr.size());
}

}}

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

#ifndef GUARD_OGA_HANDLERS_HANDLER_HPP_INCLUDED
#define GUARD_OGA_HANDLERS_HANDLER_HPP_INCLUDED

namespace oga {

struct message_args {
};

struct message_channel {
};

struct handler_description {
    // Meta Data
    char const * name;
    char const * version;
    char const * subscribes_to;

    // Configuration
    int timer_interval;

    // Instance Data
    void * user_data;
    void (*on_message)(char const * name, message_args const *);
    void (*on_timer)();
    void (*set_channel)(message_channel * channel);
    void (*release)();
};

class handler_base {
public:
    virtual ~handler_base();

    virtual void on_message(std::string const & message, message_args const & args) = 0;
    virtual void on_timer() = 0;
    virtual void release() = 0;
    virtual void set_channel(message_channel * channel);
};

class plugin_handler : public handler_base {
public:
    plugin_handler(handler_description const & description);
    virtual ~plugin_handler();

    virtual void on_message(std::string const & message, message_args const & args) {
        if(description_.on_message) {
            description_.on_message(message.c_str(), &args);
        }
    }

    virtual void on_timer() {
        if(description_.on_timer) {
            description_.on_timer();
        }
    }

    virtual void release() {
        if(description_.release) {
            description_.release();
        }
    }
    virtual void set_channel(message_channel * channel) {
        if(description_.set_channel) {
            description_.set_channel(channel);
        }
    }
private:
    handler_description description_;
};

}

#endif //GUARD_OGA_HANDLERS_HANDLER_HPP_INCLUDED

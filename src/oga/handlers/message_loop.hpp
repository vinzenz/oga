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

#ifndef GUARD_OGA_HANDLERS_MESSAGE_LOOP_HPP_INCLUDED
#define GUARD_OGA_HANDLERS_MESSAGE_LOOP_HPP_INCLUDED

#include <oga/comm/connection.hpp>
#include <oga/handlers/handler.hpp>
#include <oga/base/timer_queue.hpp>

namespace oga {
	class message_loop : public thread {
		oga::timer_queue timer_;
		oga::comm::connection & connection_;
		oga::message_manager messages_;
	public:
		message_loop(oga::comm::connection & connection)
		: thread("message_loop")
        , connection_(connection)
		, messages_()
		{
            start();
        }

        void startup() {
            start();
        }

        void shutdown() {
            stop();
            connection_.close();
            wait();
        }

        ~message_loop() {
            shutdown();
        }

		void add_timer(int64_t interval, oga::util::shared_ptr<handler> handler) {
			timer_.add(interval, handler.ptr());
		}

		void del_timer(handler* h) {
			timer_.remove(h);
		}

		void add_command(std::string const & name, oga::util::shared_ptr<handler> handler) {
			messages_.set(name, handler);
		}
		void del_command(std::string const & name) {
			messages_.drop(name);
		}
    private:
        void run() {
            while (!should_stop()) {
                oga::proto::json::object message;
                error_type result = connection_.receive(message);
                if (result.code() == 0) {
                    if (message.is_string("__name__")) {
                        std::string name = message["__name__"].get_string();
                        message.erase("__name__");
                        try {
                            messages_.dispatch_message(name, message);
                        }
                        catch (...) {
                            OGA_LOG_ERROR(log::get("message_loop"), "Unhandled exception caught while handling: {0} args: {1}") % name % message;
                        }
                    }
                }
                else {
                    OGA_LOG_INFO(log::get("message_loop"), "Failed to receive message from connection. Code: {0} Message: {1}") % result.code() % result.message();
                    this_thread::sleep(1000);
                }
            }
        }
	};
}

#endif // GUARD_OGA_HANDLERS_MESSAGE_LOOP_HPP_INCLUDED
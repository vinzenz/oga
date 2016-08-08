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

#ifndef GUARD_OGA_HANDLERS_MESSAGE_LOOP_HPP_INCLUDED
#define GUARD_OGA_HANDLERS_MESSAGE_LOOP_HPP_INCLUDED

#include <oga/comm/connection.hpp>
#include <oga/handlers/handler.hpp>
#include <oga/base/timer_queue.hpp>

namespace oga {
	class message_loop {
		oga::timer_queue timer_;
		oga::comm::connection & connection_;
		oga::message_manager messages_;
	public:
		message_loop(oga::comm::connection & connection)
			: connection_(connection)
			, messages_()
		{}

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
	};
}

#endif // GUARD_OGA_HANDLERS_MESSAGE_LOOP_HPP_INCLUDED
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

#include <oga/base/errors.hpp>
#include <oga/proto/json/json.hpp>

#include <string>
#include <map>

namespace oga {

	class handler {
	public:
		virtual ~handler() {}
		virtual error_type call(oga::proto::json::object const & args = oga::proto::json::object()) = 0;
	};

	class message_manager {
		std::map<std::string, oga::util::shared_ptr<handler> > handlers_;
	public:
		void set(std::string const & name, oga::util::shared_ptr<handler> handler) {
			handlers_[name] = handler;
		}

		void drop(std::string const & name) {
			handlers_.erase(name);
		}

		void dispatch_message(
			std::string const & name,
			oga::proto::json::object const & args) {
			apply_call(handlers_.find(name), args);
		}
	protected:
		template<typename T>
		void apply_call(T iter, oga::proto::json::object const & args) {
			if (iter != handlers_.end()) {
				iter->second->call(args);
			}
		}
	};
}

#endif //GUARD_OGA_HANDLERS_HANDLER_HPP_INCLUDED

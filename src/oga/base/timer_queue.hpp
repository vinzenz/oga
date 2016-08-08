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

#ifndef GUARD_OGA_BASE_TIMER_QUEUE_HPP_INCLUDED
#define GUARD_OGA_BASE_TIMER_QUEUE_HPP_INCLUDED

#include <oga/base/types.hpp>
#include <oga/util/shared_ptr.hpp>

#include <vector>

namespace oga {
	class timer_queue_triggerable {
	public:
		virtual void * id() const = 0;
		virtual void trigger() = 0;
	};

	struct priority_queue;

	template< typename Implementation >
	class timer_queue_triggerable_wrapper : public timer_queue_triggerable {
		Implementation* implementation_;
	public:
		timer_queue_triggerable_wrapper(Implementation* implementation)
		: implementation_(implementation)
		{}

		void * id() const {
			return implementation_;
		}

		void trigger() {
			implementation_->call();
		}
	};
	
	class timer_queue {		
	public:
		timer_queue();		
		virtual ~timer_queue();
		
		template< typename Callable >
		void add(int64_t interval, Callable * callable) {
			add(interval, oga::util::shared_ptr<timer_queue_triggerable>(
				new timer_queue_triggerable_wrapper<Callable>(callable)));
		}

		void add(int64_t interval, oga::util::shared_ptr<timer_queue_triggerable> triggerable);				
		void remove(void*);
	protected:
		uint64_t started_ = 0;
		oga::util::shared_ptr<priority_queue> queue_;
	private:
		timer_queue(timer_queue const &);
		timer_queue & operator=(timer_queue const &);
	};
}

#endif //GUARD_OGA_BASE_TIMER_QUEUE_HPP_INCLUDED

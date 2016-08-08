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

#include <oga/base/timer_queue.hpp>
#include <queue>
#include <oga/base/thread.hpp>
#include <time.h>
#if defined(WIN32)
#include <windows.h>
#endif

namespace oga {
#if defined(WIN32)
    namespace {
        struct perf_timer {
            int64_t frequency_;
            perf_timer() {
                LARGE_INTEGER li;
                QueryPerformanceFrequency(&li);
                frequency_ = li.QuadPart;
            }

            int64_t get() {
                LARGE_INTEGER li;
                QueryPerformanceCounter(&li);
                return li.QuadPart  / frequency_;
            }
        };
    }
#endif
	int64_t get_time_in_millis() {
#if defined(WIN32)
        static perf_timer timer;
        return timer.get();
#else
		timespec ts_start;
		clock_gettime(CLOCK_MONOTONIC, &ts_start);
		return (int64_t(ts_start.tv_sec) * 1000LL)
			 + (int64_t(ts_start.tv_nsec) / 1000000LL);
#endif
	}

	struct timer_queue_entry {
		int64_t interval;
		int64_t next_time;
		oga::util::shared_ptr<timer_queue_triggerable> triggerable;
	};

	struct by_next_time {
		template< typename Element >
		bool operator()(Element const & lhs, Element const & rhs) const {
			return lhs.next_time < rhs.next_time;
		}
	};

	struct priority_queue : std::priority_queue<
		timer_queue_entry,
		std::vector<timer_queue_entry>,
		by_next_time
	>
	{
		oga::critical_section lock;

		container_type & container() {
			return c;
		}
	};


	timer_queue::timer_queue()
	: queue_(new priority_queue())
	{}

	timer_queue::~timer_queue()
	{}

	void timer_queue::add(int64_t interval, oga::util::shared_ptr<timer_queue_triggerable> triggerable) {
		timer_queue_entry e;
		e.interval = interval;
		e.next_time = get_time_in_millis() + interval;
		e.triggerable = triggerable;
		scoped_lock<critical_section> lock(queue_->lock);
		queue_->push(e);
	}

	void timer_queue::remove(void * id) {
		scoped_lock<critical_section> lock(queue_->lock);
		size_t count = queue_->container().size();
		for (size_t i = 0; i < count; ++i) {
			if(queue_->container()[i].triggerable->id() == id) {
				queue_->container()[i].interval = 0;
				queue_->container()[i].next_time = get_time_in_millis();
			}
		}
	}
}

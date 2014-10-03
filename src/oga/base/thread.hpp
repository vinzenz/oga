#ifndef GUARD_OGA_BASE_THREAD_HPP_INCLUDED
#define GUARD_OGA_BASE_THREAD_HPP_INCLUDED

#include <oga/base/detail/thread.hpp>
#include <string>

namespace oga {
    namespace this_thread {
        thread_id id();
        thread_handle handle();
        std::string name();
    }
}

#endif //GUARD_OGA_BASE_THREAD_HPP_INCLUDED

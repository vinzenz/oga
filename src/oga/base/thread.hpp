#ifndef GUARD_OGA_BASE_THREAD_HPP_INCLUDED
#define GUARD_OGA_BASE_THREAD_HPP_INCLUDED

#include <oga/base/detail/thread.hpp>
#include <oga/base/errors.hpp>
#include <string>

namespace oga {
    namespace this_thread {
        thread_id id();
        thread_handle handle();
        std::string name();
    }

    class thread;
    void thread_runner(thread*);

    class thread {
        std::string const name_;
        bool stop_;
        thread_handle handle_;
    public:
        thread(std::string const & name);
        virtual ~thread();

        void start();
        void stop(bool should_wait = false);

        error_type wait(size_t const milliseconds = ~size_t(0));

        thread_handle handle() const;
    protected:
        bool should_stop() const;

        virtual void run() = 0;
        friend void oga::thread_runner(thread *);
    private:
        thread(thread const &);
        thread & operator=(thread const &);
    };

    class critical_section {
    public:
        critical_section();
        ~critical_section();

        bool try_lock() const;
        void lock() const;
        void unlock() const;

    protected:
#if defined(WIN32)
        mutable CRITICAL_SECTION lock_;
#else
        mutable pthread_mutex_t lock_;
#endif
    private:
        // Not copyable
        critical_section(critical_section const &);
        critical_section & operator=(critical_section const &);
    };

    template< typename Lockable >
    class scoped_lock {
        Lockable & lock_;
    public:
        scoped_lock(Lockable & lock) : lock_(lock) {
            lock_.lock();
        }

        ~scoped_lock() {
            lock_.unlock();
        }
    private:
        scoped_lock(scoped_lock const &);
        scoped_lock & operator=(scoped_lock const &);
    };

    class event {
    public:
        event(bool manual_reset = false);
        virtual ~event();

        bool wait(int64_t time_millis = -1);
        void set();
        void reset();
    protected:
#if defined(WIN32)
        HANDLE handle_;
#else
        bool signaled_;
        bool manual_;
        pthread_cond_t cond_;
        pthread_mutex_t lock_;
#endif
    private:
        event(event const &);
        event & operator=(event const &);
    };
}

#endif //GUARD_OGA_BASE_THREAD_HPP_INCLUDED

/**
 * @file
 * This file is part of the Mini Thread Library (https://github.com/RoseLeBlood/MiniThread ).
 * @author Copyright (c) 2021 Amber-Sophia Schroeck
 * @par License
 * The Mini Thread Library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, version 3, or (at your option) any later version.
 *
 * The Mini Thread Library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the Mini Thread  Library; if not, see
 * <https://www.gnu.org/licenses/>.
 */
#ifndef MINLIB_MN_MUTEX_
#define MINLIB_MN_MUTEX_

#include "Mutex.hpp"
#include "timed_lock.hpp"
#include "autolock.hpp"
#include "chrono.hpp"


namespace mn {
    template <class TMUTEX>
    class basic_timed_mutex : public basic_timed_lock<TMUTEX> {
    public:
        template <class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time)
        { return try_lock_until(mn::chrono::system_clock::now() + __d); }

        template <class Clock, class Duration>
        bool try_lock_until(const mn::chrono::time_point<Clock, Duration>& abs_time){
            basic_autolock<TLOCK> lock(m_lockObject);
            bool no_timeout = Clock::now() < abs_time;

            mn::ext::basic_convar_task* task = mn::ext::basic_convar_task::get_self();

            while (no_timeout && m_bLocked)
                no_timeout = task->wait(m_cv, m_mutex, timeout) == ERR_SPINLOCK_OK;

            delete task;

            if (!m_bLocked) {
                m_bLocked = true;
                return true;
            }
            return false;
        }
    };

    using mutex = basic_mutex;
    using timed_mutex = basic_timed_mutex<basic_mutex>;

    using recursive_mutex = basic_recursive_mutex;
    using recursive_timed_mutex = basic_timed_mutex<basic_recursive_mutex>;
    
    struct defer_lock_t {};
    struct try_to_lock_t {};
    struct adopt_lock_t {};

    inline constexpr defer_lock_t  defer_lock{};
    inline constexpr try_to_lock_t try_to_lock{};
    inline constexpr adopt_lock_t  adopt_lock{};

    template <class Mutex>
    class lock_guard {
    public:
        using mutex_type = Mutex;

        explicit lock_guard(mutex_type& m) : m_ref_lock(m) {
            assert( (m_ref_lock.lock(portMAX_DELAY) != NO_ERROR) );
        }

        lock_guard(mutex_type &m, unsigned long xTicksToWait)
            : m_ref_lock(m) {
            assert( (m_ref_lock.lock(xTicksToWait) != NO_ERROR) );
        }

        lock_guard(mutex_type& m, adopt_lock_t) : lock_guard(m) { }

        ~lock_guard() {
            m_ref_lock.unlock();
        }

        lock_guard(lock_guard const&) = delete;
        lock_guard& operator=(lock_guard const&) = delete;
    private:
        mutex_type m_ref_lock;
    };

    template <class Mutex>
    class unique_lock {
    public:
        using mutex_type = Mutex;

        unique_lock() noexcept : m_mut(nullptr), m_owns(false) { }
        explicit unique_lock(mutex_type& m, unsigned int timeout = MN_THREAD_CONFIG_TIMEOUT_MUTEX_DEFAULT) 
            : m_mut(m), m_owns(true) {m_mut->lock(timeout);}

        unique_lock(mutex_type& m, defer_lock_t) noexcept
            : m_mut(m), m_owns(false) { } 

        unique_lock(mutex_type& m, try_to_lock_t)
            : m_mut(m), m_owns(m.try_lock()) { } 
        unique_lock(mutex_type& m, adopt_lock_t) 
            : m_mut(m), m_owns(false) { }  

        template <class Clock, class Duration>
        unique_lock(mutex_type& m, const chrono::time_point<Clock, Duration>& abs_time)
            : m_mut(m), m_owns(m.try_lock_until(abs_time)) {}

        template <class Rep, class Period>
        unique_lock(mutex_type& m, const chrono::duration<Rep, Period>& rel_time)
             :  m_mut(m), m_owns(m.try_lock_for(rel_time)) {}

        ~unique_lock() { if (m_owns) m_mut->unlock(); }

        unique_lock(unique_lock const&) = delete;
        unique_lock& operator=(unique_lock const&) = delete;

        unique_lock(unique_lock&& u) noexcept;
        unique_lock& operator=(unique_lock&& u) noexcept;

        void lock(unsigned int timeout = MN_THREAD_CONFIG_TIMEOUT_MUTEX_DEFAULT) {
            if (m_mut == nullptr) return;
            if (m_owns) return;

            m_mut->lock(timeout);
            m_owns = true;
        }
        bool try_lock() {
            if (m_mut == nullptr) return;
            if (m_owns) return;

            m_owns = m_mut->lock(0) == 0;
        }

        template <class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) {
            if (m_mut == nullptr) return;
            if (m_owns) return;

            m_owns = m_mut->try_lock_for(rel_time);
        }
        template <class Clock, class Duration>
        bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) {
            if (m_mut == nullptr) return;
            if (m_owns) return;

            m_owns = m_mut->try_lock_until(abs_time);
        }

        void unlock() {
            if (!__owns_) return;
            m_mut->unlock();
            m_owns = false;
        }

        void swap(unique_lock& u) noexcept {
            mn::swap(m_mut, u.m_mut);
            mn::swap(m_owns, u.m_owns);
        }
        mutex_type* release() noexcept {
            mutex_type* _mut = m_mut;
            m_mut = nullptr;
            m_owns = false;
            return _mut;
        }

        bool owns_lock() const noexcept             { return m_owns; }
        explicit operator bool () const noexcept    { return m_owns; }
        mutex_type* mutex() const noexcept          { return m_mut; }
    private:
        mutex_type* m_mut;
        bool        m_owns;
    };
    template <class _Mutex> 
    inline void swap(unique_lock<_Mutex>& a, unique_lock<_Mutex>& b) noexcept
        { a.swap(b); }
}

#endif
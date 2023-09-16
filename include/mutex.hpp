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
#ifndef MINLIB_ESP32_MUTEX_
#define MINLIB_ESP32_MUTEX_

#include "config.hpp"
#include "def.hpp"

#include "basic_semaphore.hpp"
#include "error.hpp"
#include "chrono.hpp"


namespace mofw {
  /**
   *  Base wrapper class around FreeRTOS's implementation of mutexes.
   *  These objects are not recursively acquirable. Calling lock() twice from
   *  the same Thread (i.e. task) will deadlock.
   *
   * @ingroup mutex
   * @ingroup lock
   */
  class basic_mutex : public basic_semaphore {
  public:
    /**
     * Create the mutex
     *
     * @note When enabled the config item MN_THREAD_CONFIG_USE_LOCK_CREATE then throw on error
     * the lockcreate_exception exceptions and the config item MN_THREAD_CONFIG_DEBUG
     * enabled, then with debug informations.
     * When the config item MN_THREAD_CONFIG_USE_LOCK_CREATE disabled then get the created error code
     * with basic_semaphore::get_error()
     */
    basic_mutex();
    basic_mutex(const basic_mutex& other);
    /**
     * Destrutor - destroy the mutex
     */
    virtual ~basic_mutex();

    /**
     *  Lock the Mutex.
     *
     *  @param Timeout How long to wait to get the Lock until giving up. (default = 0xffffffffUL)
     *  @return ERR_MUTEX_OK if the Lock was acquired, ERR_MUTEX_LOCK if it timed out.
     *  or ERR_MUTEX_NOTINIT when mutex not created
     */
    virtual int lock(unsigned int timeout = MN_THREAD_CONFIG_TIMEOUT_MUTEX_DEFAULT);

    /**
     *  Unlock the Mutex.
     *
     *  @return ERR_MUTEX_OK if the Lock was released, ERR_MUTEX_UNLOCK if it failed.
     *  or ERR_MUTEX_NOTINIT when mutex not created
     */
    virtual int unlock();

    virtual bool try_lock() { lock(0); return m_isLocked; }
  };

  /**
   *  Recursive usage Mutex.
   *
   *  By default calls to Lock these objects block forever, but this can be
   *  changed by simply passing in a argument to the lock() method.
   *  These objects are recursively acquirable. Calling lock() twice from
   *  the same Thread (i.e. task) works fine. The caller just needs to be sure to
   *  call unlock() as many times as lock().
   *
   *  @note Recursive mutexes use more resources than standard mutexes. You
   *        should be sure that you actually need this type of synchronization
   *        before using it.
   *
   * @ingroup mutex
   * @ingroup lock
   */
  class basic_recursive_mutex : public basic_mutex {
  public:
    /**
     * Create the recursive mutex
     *
     * @note When enabled the config item MN_THREAD_CONFIG_USE_LOCK_CREATE then throw on error
     * the lockcreate_exception exceptions and the config item MN_THREAD_CONFIG_DEBUG
     * enabled, then with debug informations.
     * When the config item MN_THREAD_CONFIG_USE_LOCK_CREATE disabled then get the error code
     * with basic_semaphore::get_error()
     */
    basic_recursive_mutex();

    basic_recursive_mutex(const basic_recursive_mutex&) = delete;
    basic_recursive_mutex& operator=(const basic_recursive_mutex&) = delete;
    /**
     *  Lock the Mutex.
     *
     *  @param Timeout How long to wait to get the Lock until giving up. (default = 0xffffffffUL)
     *  @return ERR_MUTEX_OK if the Lock was acquired, ERR_MUTEX_LOCK if it timed out.
     *  or ERR_MUTEX_NOTINIT when mutex not created
     *
     *  @note use 'xSemaphoreTakeRecursiveFromISR' in ISR context or 'xSemaphoreTakeRecursive' in all other
     */
    virtual int lock(unsigned int timeout = MN_THREAD_CONFIG_TIMEOUT_MUTEX_DEFAULT);

    /**
     *  Unlock the Mutex.
     *
     *  @return ERR_MUTEX_OK if the Lock was released, ERR_MUTEX_UNLOCK if it failed.
     *  or ERR_MUTEX_NOTINIT when mutex not created
     *
     *  @note use 'xSemaphoreGiveRecursiveFromISR' in ISR context or 'xSemaphoreGiveRecursive' in all other
     */
    virtual int unlock();

    
  };


  using mutex_t = basic_mutex;

  template <class TMUTEX>
    class basic_timed_mutex : public basic_timed_lock<TMUTEX> {
    public:
        template <class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time)
        { return try_lock_until(mofw::chrono::system_clock::now() + __d); }

        template <class Clock, class Duration>
        bool try_lock_until(const mofw::chrono::time_point<Clock, Duration>& abs_time){
            basic_autolock<TLOCK> lock(m_lockObject);
            bool no_timeout = Clock::now() < abs_time;

            mofw::basic_convar_task* task = mofw::basic_convar_task::get_self();

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
            mofw::swap(m_mut, u.m_mut);
            mofw::swap(m_owns, u.m_owns);
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

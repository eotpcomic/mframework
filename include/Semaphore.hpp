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
#ifndef MINLIB_MN_SEM_
#define MINLIB_MN_SEM_

#include "semaphore.hpp"
#include "counting_semaphore.hpp"
#include "timed_lock.hpp"
#include "autolock.hpp"
#include "chrono.hpp"


namespace mn {
    template<ptrdiff_t TLESSTMAXVAL = MN_THREAD_CONFIG_CSEMAPHORE_MAX_COUNT>
     class counting_semaphore {
        static_assert(TLESSTMAXVAL >= 0);
        static_assert(TLESSTMAXVAL <= MN_THREAD_CONFIG_CSEMAPHORE_MAX_COUNT);

        ~counting_semaphore() : m_sem(0, TLESSTMAXVAL) { }

        counting_semaphore(const counting_semaphore&) = delete;
        counting_semaphore& operator=(const counting_semaphore&) = delete;

        static constexpr ptrdiff_t max() noexcept { return TLESSTMAXVAL; }

        void release(unsigned int timeout = MN_THREAD_CONFIG_TIMEOUT_SEMAPHORE_DEFAULT)
            { m_sem.unlock(timeout); }
        
        void acquire(unsigned int timeout = MN_THREAD_CONFIG_TIMEOUT_SEMAPHORE_DEFAULT) 
            { m_sem.lock(timeout); }
    private:
        basic_counting_semaphore m_sem;
    };

    using binary_semaphore = mn::binary_semaphore_t;
}

#endif
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
#ifndef _MINLIB_AUTOLOCK_H_
#define _MINLIB_AUTOLOCK_H_

#include "config.hpp"
#include "lock.hpp"

#include "mutex.hpp"
#include "semaphore.hpp"
#include "null_lock.hpp"


#ifndef portMAX_DELAY
#define portMAX_DELAY 0xffffffffUL
#endif

namespace mofw {
  /**
   * A autolock type for counting_semaphore_t objects
   */
  using autocsemp_t = basic_autolock<counting_semaphore_t>;

  /**
   * A autolock type for binary_semaphore_t objects
   */
  using autobinsemp_t = basic_autolock<binary_semaphore_t>;

  /**
   * A autolock type for mutex_t objects
   */
  using automutx_t = basic_autolock<mutex_t>;

  #if (MN_THREAD_CONFIG_RECURSIVE_MUTEX == MN_THREAD_CONFIG_YES)
  //using autoremutx_t = basic_autolock<remutex_t>;
  #endif

  #if MN_THREAD_CONFIG_LOCK_TYPE == MN_THREAD_CONFIG_MUTEX
    using LockType_t = mofw::mutex_t;
  #elif MN_THREAD_CONFIG_LOCK_TYPE == MN_THREAD_CONFIG_BINARY_SEMAPHORE
    using LockType_t = mofw::binary_semaphore_t;
  #elif MN_THREAD_CONFIG_LOCK_TYPE == MN_THREAD_CONFIG_COUNTING_SEMAPHORE
    using LockType_t = mofw::counting_semaphore_t;
  //#elif MN_THREAD_CONFIG_LOCK_TYPE == MN_THREAD_CONFIG_RECURSIVE_MUTEX
  //  using LockType_t = remutex_t;
  #endif

  using autolock_t = basic_autolock<LockType_t>;

  template <class TLOCK = LockType_t>
  int lock(TLOCK& m1, unsigned int timeout) { return m1.lock(timeout); }

  template <class TLOCK = LockType_t>
  int unlock(TLOCK& m1) { return m1.unlock(); }

  template <class TLOCK = LockType_t>
  bool try_lock(TLOCK& m1) { return m1.try_lock(); }
}

#endif

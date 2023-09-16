/**
 * @file
 * This file is part of the Mini Thread Library (https://github.com/RoseLeBlood/MiniThread ).
 * @author Copyright (c) 2018 Amber-Sophia Schroeck
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
#ifndef MINLIB_ESP32_COUNTING_SEMAPHORE_
#define MINLIB_ESP32_COUNTING_SEMAPHORE_

#include "config.hpp"
#include "basic_semaphore.hpp"

namespace mofw {
  /**
   * @ingroup semaphore
   * @ingroup lock
   */
  template<ptrdiff_t TLESSTMAXVAL = MN_THREAD_CONFIG_CSEMAPHORE_MAX_COUNT>
  class basic_counting_semaphore : public basic_semaphore {
  public:
    static_assert(TLESSTMAXVAL >= 0);
    static_assert(TLESSTMAXVAL <= MN_THREAD_CONFIG_CSEMAPHORE_MAX_COUNT);
    /**
     * Create the counting semaphore
     *
     * @note When enabled the config item MN_THREAD_CONFIG_USE_LOCK_CREATE then throw on error
     * the lockcreate_exception exceptions and the config item MN_THREAD_CONFIG_DEBUG
     * enabled, then with debug informations.
     * When the config item MN_THREAD_CONFIG_USE_LOCK_CREATE disabled then get the created error code
     * with basic_semaphore::get_error()
     *
     *
     *  @param count Must not be greater than maxCount.
     */
    basic_counting_semaphore(int count = MN_THREAD_CONFIG_CSEMAPHORE_MIN_COUNT)
      : basic_semaphore() {
      if ( (TLESSTMAXVAL < count) ) {
        MN_THROW_LOCK_EXP(ERR_SPINLOCK_BAD_INITIALCOUNT);
      } else {
      #if( configSUPPORT_STATIC_ALLOCATION == 1 )
        m_pSpinlock = xSemaphoreCreateCountingStatic(TLESSTMAXVAL, count, &m_SemaphoreBasicBuffer);
      #else
        m_pSpinlock = xSemaphoreCreateCounting(TLESSTMAXVAL, count);
      #endif

        if (m_pSpinlock) {
          unlock();
        } else {
          MN_THROW_LOCK_EXP(ERR_SPINLOCK_CANTCREATESPINLOCK);
        }
      }
  }

  basic_counting_semaphore(const basic_counting_semaphore&) = delete;
  basic_counting_semaphore& operator=(const basic_counting_semaphore&) = delete;

  static constexpr ptrdiff_t max() noexcept { return TLESSTMAXVAL; }

	
  virtual ~basic_counting_semaphore() {
    if(m_pSpinlock) {
      vSemaphoreDelete(m_pSpinlock);
    } else {
      MN_THROW_LOCK_EXP(ERR_MUTEX_NOTINIT);
    }
  }


    /**
     * Get the current count value.
     *
     * @return The current count and 0 if the semaphore is not available.
     */
    int get_count() const {
      return uxQueueMessagesWaiting((QueueHandle_t)m_pSpinlock);
    }

    void release(unsigned int timeout = MN_THREAD_CONFIG_TIMEOUT_SEMAPHORE_DEFAULT)
            { m_sem.unlock(timeout); }
        
    void acquire(unsigned int timeout = MN_THREAD_CONFIG_TIMEOUT_SEMAPHORE_DEFAULT) 
        { m_sem.lock(timeout); }

  };

  
}

#endif

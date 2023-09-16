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
#ifndef _MINLIB_SHARED_LOCK_H_
#define _MINLIB_SHARED_LOCK_H_

#include "shared.hpp"
#include "Mutex.hpp"
#include "lock.hpp"

namespace mofw {

    class shared_mutex {
    public:
        /**
         * Resource constructor.
         * @param value sets the initial value of the resource.
         */
        shared_mutex () { }
        /**
         * Get the value of the shared resource.
         * @return The value of the shared resource.
         */
        void lock () const {
            m_pReadWriteLock.lock();
        }

    protected:
        /**
         * the controll mutex
         */
        mutex_t m_pReadWriteLock;
    };


    template<typename TLOCK>
    class shared_lock {

    };

}


#endif
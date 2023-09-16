/*
*This file is part of the Mini Thread Library (https://github.com/RoseLeBlood/MiniThread ).
*Copyright (c) 2021 Amber-Sophia Schroeck
*
*The Mini Thread Library is free software; you can redistribute it and/or modify
*it under the terms of the GNU Lesser General Public License as published by
*the Free Software Foundation, version 3, or (at your option) any later version.

*The Mini Thread Library is distributed in the hope that it will be useful, but
*WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
*General Public License for more details.
*
*You should have received a copy of the GNU Lesser General Public
*License along with the Mini Thread  Library; if not, see
*<https://www.gnu.org/licenses/>;.
*/
#ifndef _MINLIB_WAIT_STATE_H_
#define _MINLIB_WAIT_STATE_H_

#include "../task.hpp"
#include "atomic_flags.hpp"

namespace mofw {
    template <class TTYPE /* =  basic_atomic_impl<uint64_t>*/ >
    struct basic_wait_state {
        TTYPE m_waiters{0};
        mutex_t m_locked;
        mofw::convar_t m_convar;
        uint64_t m_version;

        // Get the wait state for a given address.
        static basic_wait_state &for_address(volatile void *__address) noexcept {
            constexpr uintptr_t count = 16;
            static basic_wait_state w[count];
            return w[(reinterpret_cast<uintptr_t>(__address) >> 2) % count];
        }

        void notify(unsigned int timeout) noexcept {
            if (m_waiters.load() != 0) {
                m_locked.lock(timeout);
                m_version++;
                m_locked.unlock();
            }
            m_convar.notify_all();
        }

        template <typename TPred>
        void wait(TPred pred, unsigned int timeout) {
            for (int i = 0; i < 10; i++) {
                if (pred())
                    return;
                taskYIELD();
            }

            m_waiters.fetch_add(1, mofw::memory_order::SeqCst);

            uint64_t prev = [&] {
                m_locked.lock(timeout);
                uint64_t ver = m_version;
                m_locked.unlock();

                return ver;
            }();

            while (!pred()) {
                m_locked.lock(timeout); 
                if (m_version == prev) {
                    m_convar.wait(m_locked, timeout );
                }
                prev = m_version;
            }
            m_waiters.fetch_sub(1, mofw::memory_order::Release);
        }
    };
}

#endif
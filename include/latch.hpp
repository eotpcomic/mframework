/**
 * @file
 * This file is part of the Mini Thread Library (https://github.com/RoseLeBlood/MiniThread ).
 * @author Copyright (c) 2023 Amber-Sophia Schroeck
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
#ifndef MINLIB_latch_H_
#define MINLIB_latch_H_

#include "algorithm.hpp"
#include "atomic.hpp"


namespace mofw {
    class latch {
    public:
        static constexpr ptrdiff_t max() { return UINT64_MAX; }

        constexpr explicit latch(ptrdiff_t expected) : m_val(expected) { }

        ~latch() = default;
        latch(const latch&) = delete;
        latch& operator=(const latch&) = delete;

        inline void  count_down(ptrdiff_t i = 1, unsigned int timeout) {
            auto const temp = m_val.fetch_sub(i, memory_order::Release);
            if (temp == i)
                m_val.notify_all(timeout);
        }

        bool try_wait() const {
            return m_val.load(memory_order::Acquire) == 0;
        }
        void wait(unsigned int timeout) const noexcept  {
            auto const pred = [this] { return this->try_wait(); };
            auto& s = atomic_long::waitstate_type::for_address(&m_val);
            
            s.wait(pred, timeout);
        }
        void arrive_and_wait(ptrdiff_t up = 1, unsigned int timeout) {
            count_down(up, timeout);
            wait(timeout);
        }
    private:
        atomic_long m_val;

    };
}

#endif
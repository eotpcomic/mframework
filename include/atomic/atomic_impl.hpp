// *** ADDED BY HEADER FIXUP ***
#include <cstdlib>
// *** END ***
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
#ifndef _MINLIB_5ac13f75_2f13_4bb7_acae_7b8f27f78e20_IMPL_H_
#define _MINLIB_5ac13f75_2f13_4bb7_acae_7b8f27f78e20_IMPL_H_

#if defined(__GNUC__) || defined(__clang__) || defined(__xlc__)
    #include "atomic_gcc.hpp"
    #define BASE_ATOMIC_TYPE mofw::basic_atomic_gcc
    #define BASE_ATOMIC_SMART_POINTER mofw::basic_smart_pointer_atomic_gcc

    #undef ATOMIC_VAR_INIT
    #define	ATOMIC_VAR_INIT(value)		{ .__tValue = (value) }
    #define	atomic_init(obj, value)		((void)((obj)->__tValue = (value)))
    #define ATOMIC_FLAG_INIT            { 0 }
#else
#error Unsupported compiler / system.
#endif

#include "waitstate.hpp"

namespace mofw {
    
    namespace internal {
        template<typename TATOMIC, class TSTATE>
        class notify_token {
        public:
            void notify_one(unsigned int timeout) noexcept { m_state->notify(timeout); }
            void notify_all(unsigned int timeout) noexcept { m_state->notify(timeout); }

            explicit notify_token(TSTATE& state) 
                : m_state(&state) {}
        private:
            TSTATE* m_state;
        };
    }

    template <typename T>
    class basic_atomic_impl : public BASE_ATOMIC_TYPE<T> {
    public:
        using base_type = BASE_ATOMIC_TYPE<T>;
        using value_type = typename base_type::value_type;
        using difference_type = typename base_type::difference_type;
        using self_type = basic_atomic_impl<value_type>;
        using waitstate_type = basic_wait_state<basic_atomic_impl<uint64_t>>;
        using notify_type = mofw::internal::notify_token<self_type, waitstate_type>;
        using vnotify_type = mofw::internal::notify_token<volatile self_type, waitstate_type>;

        basic_atomic_impl()  = default;
        ~basic_atomic_impl()  = default;

        basic_atomic_impl(const self_type&) = delete;
        basic_atomic_impl& operator=(const self_type&) = delete;
        basic_atomic_impl& operator=(const self_type&) volatile = delete;

        constexpr basic_atomic_impl(value_type value)  : base_type(value) { }


        void notify_one(unsigned int timeout) 
            { get_notify_token().notify_one(timeout); }

        void notify_one(unsigned int timeout) volatile  
            { get_notify_token().notify_one(timeout); }
  
        void notify_all(unsigned int timeout)  
            { get_notify_token().notify_all(timeout); }
            
        void notify_all(unsigned int timeout) volatile 
            { get_notify_token().notify_all(timeout); }

        notify_type get_notify_token()  {
            waitstate_type& temp = waitstate_type::for_address(this);
            return notify_type{temp};
        }

        vnotify_type get_notify_token() volatile {
            waitstate_type& temp = waitstate_type::for_address(this);
            return notify_type{temp};
        }

        void wait(T old, mofw::memory_order mo = mofw::memory_order::SeqCst, unsigned int timeout = MN_THREAD_CONFIG_TIMEOUT_MUTEX_DEFAULT) const {
            
            auto pred = [mo, old, this]() { return this->load(mo) != old; };
            auto& s = waitstate_type::for_address(this);
            s.wait(pred, timeout);
        }

        void wait(T old, mofw::memory_order mo = mofw::memory_order::SeqCst, unsigned int timeout = MN_THREAD_CONFIG_TIMEOUT_MUTEX_DEFAULT) const volatile {
            
            auto pred = [mo, old, this]() { return this->load(mo) != old; };
            auto& s = waitstate_type::for_address(this);
            s.wait(pred, timeout);
        }
    };

    
}

#endif

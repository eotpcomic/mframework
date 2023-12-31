/*
*This file is part of the Mini Thread Library (https://github.com/RoseLeBlood/MiniThread ).
*Copyright (c) 2018-2020 Amber-Sophia Schroeck
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
*<https://www.gnu.org/licenses/>.
*/
#include "config.hpp"

#include "convar.hpp"
#include "convar_task.hpp"
#include "task_utils.hpp"

namespace mofw  {

        //-----------------------------------
        //  construtor
        //-----------------------------------
        basic_condition_variable::basic_condition_variable()
            : m_mutex(), m_waitList() {
        }

        //-----------------------------------
        //  add_list
        //-----------------------------------
        void basic_condition_variable::add_list(basic_task *thread) {
            automutx_t autolock(m_mutex);

            m_waitList.push_back(thread);
        }

        //-----------------------------------
        //  signal
        //-----------------------------------
        void basic_condition_variable::signal() {
            automutx_t autolock(m_mutex);

            if ( !m_waitList.empty() ) {
                basic_task *thr = m_waitList.front();
                m_waitList.pop_front();

				thr->signal();
            }
        }

        //-----------------------------------
        //  broadcast
        //-----------------------------------
        void basic_condition_variable::broadcast() {
            automutx_t autolock(m_mutex);

            while ( !m_waitList.empty() ) {
                basic_task *thr = m_waitList.front();
                m_waitList.pop_front();

                thr->signal();
            }
        }

        int basic_condition_variable::wait(mofw::mutex_t& mx, TickType_t timeOut) {
            basic_task *thr =  basic_task::get_self();
            return thr->wait(*this, mx, timeOut);
        }
}



#endif

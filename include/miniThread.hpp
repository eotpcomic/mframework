/**
 * @file
 * @brief The basic include file for using this library.
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

#ifndef __LIBMIN_THREAD_H_
#define __LIBMIN_THREAD_H_

#include <cstring>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include "config.hpp"
#include "def.hpp"
#include "version.hpp"
#include "autolock.hpp"
#include "micros.hpp"
#include "task.hpp"
#include "tasklet.hpp"
#include "eventgroup.hpp"

#include "critical.hpp"
#include "timer.hpp"

#if MN_THREAD_CONFIG_CONDITION_VARIABLE_SUPPORT == MN_THREAD_CONFIG_YES
#include "convar.hpp"
#include "convar_task.hpp"
#endif

#include "queue/queue.hpp"
#include "queue/binaryqueue.hpp"
#include "queue/deque.hpp"
#include "queue/workqueue.hpp"

#include "ringbuffer.hpp"
#include "shared.hpp"

#include "atomic.hpp"
#include "safecounter.hpp"
#include "pointer.hpp"

#if MN_THREAD_CONFIG_PREVIEW_FUTURE == MN_THREAD_CONFIG_YES

#endif

namespace mofw {
    void panic();
}

#endif

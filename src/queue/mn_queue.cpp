/*
*This file is part of the Mini Thread Library (https://github.com/RoseLeBlood/MiniThread ).
*Copyright (c) 2020 Amber-Sophia Schroeck
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
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "queue/queue.hpp"
#include "error.hpp"

namespace mofw {
    namespace queue {
        basic_queue::basic_queue(unsigned int maxItems, unsigned int itemSize) 
            : m_pHandle(NULL), m_imaxItems(maxItems), m_iitemSize(itemSize)  { 

        }
        basic_queue::~basic_queue() { 

        }

        int basic_queue::create() {
            if(m_pHandle != NULL) return ERR_QUEUE_ALREADYINIT;

            m_pHandle = xQueueCreate(m_imaxItems, m_iitemSize);

            return (m_pHandle != NULL) ? ERR_QUEUE_OK : ERR_QUEUE_CANTCREATE;
        }
        int basic_queue::destroy() {
            if(m_pHandle == NULL) return ERR_QUEUE_NOTCREATED;

            vQueueDelete((QueueHandle_t)m_pHandle);
            m_pHandle = NULL;

            return ERR_QUEUE_OK;
        }

        int basic_queue::enqueue(void *item, unsigned int timeout) {
            BaseType_t success;

            if(m_pHandle == NULL) return ERR_QUEUE_NOTCREATED;

            if (xPortInIsrContext()) {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;

                success = xQueueSendToBackFromISR((QueueHandle_t)m_pHandle, item, &xHigherPriorityTaskWoken);

                if(xHigherPriorityTaskWoken)
                    _frxt_setup_switch();
            } else {
                success = xQueueSendToBack((QueueHandle_t)m_pHandle, item, timeout);
            }

            return success == pdTRUE ? ERR_QUEUE_OK : ERR_QUEUE_ADD;
        }
        int basic_queue::dequeue(void *item, unsigned int timeout) {
            BaseType_t success;

            if(m_pHandle == NULL) return ERR_QUEUE_NOTCREATED;

            if (xPortInIsrContext()) {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;

                success = xQueueReceiveFromISR((QueueHandle_t)m_pHandle, item, &xHigherPriorityTaskWoken);

                if(xHigherPriorityTaskWoken)
                    _frxt_setup_switch();
            } else {
                success = xQueueReceive((QueueHandle_t)m_pHandle, item, timeout);
            }

            return success == pdTRUE ? ERR_QUEUE_OK : ERR_QUEUE_REMOVE;
        }
        int basic_queue::peek(void *item, unsigned int timeout) {
            BaseType_t success;

            if(m_pHandle == NULL) return ERR_QUEUE_NOTCREATED;

            if (xPortInIsrContext()) {
                success = xQueuePeekFromISR((QueueHandle_t)m_pHandle, item);
            } else {
                success = xQueuePeek((QueueHandle_t)m_pHandle, item, timeout);
            }

            return success == pdTRUE ? ERR_QUEUE_OK : ERR_QUEUE_PEEK;
        }

        bool basic_queue::is_empty() {
            unsigned int cnt = get_num_items();

            return cnt == 0 ? true : false;
        }
        bool basic_queue::is_full() {
            unsigned int cnt = get_left();

            return cnt == 0 ? true : false;
        }

        int basic_queue::clear() {
            xQueueReset((QueueHandle_t)m_pHandle);
            return ERR_QUEUE_OK;
        }


        unsigned int basic_queue::get_num_items() {
            return uxQueueMessagesWaiting((QueueHandle_t)m_pHandle);
        }


        unsigned int basic_queue::get_left() {
            return uxQueueSpacesAvailable((QueueHandle_t)m_pHandle);
        }
    }
}
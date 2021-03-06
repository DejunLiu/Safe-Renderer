/******************************************************************************
**
**   File:        pgw_platform_linux.c
**   Description:
**
**   Copyright (C) 2017 Luxoft GmbH
**
**   This file is part of Safe Render.
**
**   Safe Render is free software: you can redistribute it and/or
**   modify it under the terms of the GNU Lesser General Public
**   License as published by the Free Software Foundation.
**
**   Safe Render is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**   Lesser General Public License for more details.
**
**   You should have received a copy of the GNU Lesser General Public
**   License along with Safe Render.  If not, see
**   <http://www.gnu.org/licenses/>.
**
**   SPDX-License-Identifier: LGPL-3.0
**
******************************************************************************/

#include "pgw_platform.h"
#include <sys/time.h>
#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif


void pgw_mutex_create(pgw_mutex_t* mutex, pgw_mutex_id_t id)
{
    pthread_mutex_init(mutex, NULL);
}

pgw_bool_t pgw_mutex_lock(pgw_mutex_t* mutex)
{
    pthread_mutex_lock(mutex);
    return pgw_true;
}

pgw_bool_t pgw_mutex_unlock(pgw_mutex_t* mutex)
{
    pthread_mutex_unlock(mutex);
    return pgw_true;
}

void pgw_mutex_destroy(pgw_mutex_t* mutex)
{
    pthread_mutex_destroy(mutex);
}

void pgw_event_create(pgw_event_t* ev, pgw_event_id_t id)
{
    pthread_mutex_init(&ev->mutex, NULL);
    pthread_cond_init(&ev->cond, NULL);
    ev->created = 1;
    ev->signalled = 0;
}

void pgw_event_destroy(pgw_event_t* ev)
{
    ev->signalled = 0;
    ev->created = 0;
    pthread_cond_destroy(&ev->cond);
    pthread_mutex_destroy(&ev->mutex);
}

void pgw_event_assign(pgw_event_t* to, pgw_event_t* from)
{
    to->cond = from->cond;
    to->mutex = from->mutex;
    to->signalled = from->signalled;
    to->created = from->created;
}

void pgw_event_set(pgw_event_t* ev)
{
    pthread_mutex_lock(&ev->mutex);
    ev->signalled = 1;
    pthread_mutex_unlock(&ev->mutex);
    pthread_cond_signal(&ev->cond);
}

void pgw_event_clear(pgw_event_t* ev)
{
    pthread_mutex_lock(&ev->mutex);
    ev->signalled = 0;
    pthread_mutex_unlock(&ev->mutex);
}

void pgw_event_wait(pgw_event_t* ev)
{
    pthread_mutex_lock(&ev->mutex);
    while (!ev->signalled)
    {
        pthread_cond_wait(&ev->cond, &ev->mutex);
    }
    ev->signalled = 0;
    pthread_mutex_unlock(&ev->mutex);
}

void pgw_event_wait_timeout(pgw_event_t* ev, uint32_t msTimeout)
{
    struct timespec timeToWait;
    struct timeval now;

    gettimeofday(&now,NULL);

    timeToWait.tv_sec = now.tv_sec+5;
    timeToWait.tv_nsec = (now.tv_usec+1000UL*msTimeout)*1000UL;

    pthread_mutex_lock(&ev->mutex);
    while (!ev->signalled)
    {
        pthread_cond_timedwait(&ev->cond, &ev->mutex, &timeToWait);
    }
    ev->signalled = 0;
    pthread_mutex_unlock(&ev->mutex);
}

uint32_t pgwGetMonotonicTime(void)
{
    struct timespec now;
    int res = clock_gettime(CLOCK_MONOTONIC, &now);
    uint32_t t = 0;
    if (res == 0)
    {
        t = (uint32_t)(now.tv_sec*1000 + now.tv_nsec/1000000);
    }
    return t;
}

#ifdef __cplusplus
}
#endif

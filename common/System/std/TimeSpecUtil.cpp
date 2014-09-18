/* Name: TimeSpecUtil.h
*
* Copyright(c) 2013 Loctronix Corporation
* http://www.loctronix.com
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "TimeSpecUtil.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>


void TimeSpecUtil::ToTimeVal(struct timespec *ts, struct timeval *tv)
{
  tv->tv_sec  = ts->tv_sec;
  tv->tv_usec = ts->tv_nsec / 1000;
}



void TimeSpecUtil::AddMsec(struct timespec *ts, long ms)
{
        long sec=ms/1000;
        ms=ms-sec*1000;

        // perform the addition
        ts->tv_nsec+=ms*1000000;

        // adjust the time
        ts->tv_sec+=ts->tv_nsec/1000000000 + sec;
        ts->tv_nsec=ts->tv_nsec%1000000000;
}

void TimeSpecUtil::AddNsec(struct timespec *ts, long ns)
{
        long sec=ns/1000000000;
        ns=ns - sec*1000000000;

        // perform the addition
        ts->tv_nsec+=ns;

        // adjust the time
        ts->tv_sec+=ts->tv_nsec/1000000000 + sec;
        ts->tv_nsec=ts->tv_nsec%1000000000;

}

void TimeSpecUtil::Adjust(struct timespec *ts, double dt)
{
        long sec;
        long ns;

        sec = (long) dt;
        ns = ((long) dt - sec) * 1000000000;

        while (ns < 0) {
                ns += 1000000000;
                sec--;
        }

        // perform the addition
        ts->tv_nsec+=ns;

        // adjust the time
        ts->tv_sec+=ts->tv_nsec/1000000000 + sec;
        ts->tv_nsec=ts->tv_nsec%1000000000;

}

long TimeSpecUtil::Compare(struct timespec *a, struct timespec *b)
{
        if (a->tv_sec!=b->tv_sec)
                return a->tv_sec - b->tv_sec;

        return a->tv_nsec-b->tv_nsec;
}

// computes a = a-b
void TimeSpecUtil::Subtract(struct timespec *a, struct timespec *b)
{
        a->tv_nsec = a->tv_nsec - b->tv_nsec;
        if (a->tv_nsec < 0) {
                // borrow.
                a->tv_nsec += 1000000000;
                a->tv_sec --;
        }

        a->tv_sec = a->tv_sec - b->tv_sec;
}

// convert the timespec into milliseconds (may overflow)
long TimeSpecUtil::ToMilliseconds(struct timespec *a)
{
        return a->tv_sec*1000 + a->tv_nsec/1000000;
}


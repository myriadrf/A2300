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
#ifndef TIMESPECUTIL_H_
#define TIMESPECUTIL_H_

class TimeSpecUtil
{
public:
	static void ToTimeVal(struct timespec *ts, struct timeval *tv);

	static void AddMsec(struct timespec *ts, long ms);

	static void AddNsec(struct timespec *ts, long ns);

	static void Adjust(struct timespec *ts, double dt);

	static long Compare(struct timespec *a, struct timespec *b);

	static void Subtract(struct timespec *a, struct timespec *b);

	static long ToMilliseconds(struct timespec *a);

};

#endif /* TIMESPECUTIL_H_ */
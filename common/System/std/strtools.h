/** Name: strtools.h
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

#ifndef _strtools_h__
#define _strtools_h__

char* strrev(char* szT);
char* itoa(int value, char*  str, int radix);

#define stricmp strcasecmp
#define _stricmp strcasecmp

#define __strrev strrev
#define _itoa itoa

#endif

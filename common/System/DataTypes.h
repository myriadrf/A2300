/** Name: DataTypes.h
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

#ifndef DATATYPES_H
#define DATATYPES_H

typedef char				int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;
typedef long				int32;
typedef unsigned long		uint32;

typedef unsigned char		byte;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef const char*			pcstr;
typedef char*				pstr;
typedef int					Sample;

#ifdef  __cplusplus
extern "C" {
#endif

/* Define NULL pointer value */
#ifndef NULL
	#ifdef __cplusplus
		#define NULL    0
	#else
		typedef int bool;
		#ifndef true
			#define true  (1)
			#define false (0)
		#endif
		#define NULL    ((void *)0)
	#endif
#endif

#ifdef __cplusplus
}
#endif

#endif  /* DATATYPES_H */

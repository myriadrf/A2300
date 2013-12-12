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

#ifdef WIN32
	#define SIZEOF_INT 4
	#define SIZEOF_LONG 4
#endif

typedef char				int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;

#if defined(SIZEOF_INT) && (SIZEOF_INT==4)
typedef int				int32;
typedef unsigned int		uint32;
#elif defined(SIZEOF_LONG) && (SIZEOF_LONG==4)
typedef long				int32;
typedef unsigned long		uint32;
#else
#error "SIZEOF_LONG and SIZEOF_INT are not defined; please run cmake"
#endif

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

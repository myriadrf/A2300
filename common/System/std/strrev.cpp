
/**
* Name: strrev.cpp
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

#include <string.h>
#include"strtools.h"

char* strrev(char* szT)
{
	if ( !szT )                 // handle null passed strings.
		return (char*) "";
	int i = (int) strlen(szT);
	int t = !(i%2)? 1 : 0;      // check the length of the string .
	for(int j = i-1 , k = 0 ; j > (i/2 -t) ; j-- )
	{
		char ch  = szT[j];
		szT[j]   = szT[k];
		szT[k++] = ch;
	}
	return szT;
}

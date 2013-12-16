
/**
 * 
 * Name: strtools.h
 * 
 * $Revision: $
 * $Author: $
 * $Date: $
 * 
 * DESCRIPTION:
 * USAGE:
 * 
 * This module is the proprietary property of Engenex Technologies, LLC.
 * Copyright (C) 2004 Engenex Technologies, LLC.
 * All Rights Reserved
 * 
 * 
 * 
 */

char* strrev(char* szT);
char* itoa(int value, char*  str, int radix);

#define stricmp strcasecmp
#define _stricmp strcasecmp

#define __strrev strrev
#define _itoa itoa

Visual studio 2008 solution is used to manage builds and installation packaging.  To Build the software
with Visual studio you will need to do the following:


1) Create a libusb_ext.h file in the folder <A2300 Project root>/host/include
2) Copy the code at the end of this document into the newly created file.
3) Open the visual studio solution and build the projects.   

-------------------- libusb_ext.h source ---------------

// Name: libusb_ext.h
//
// Copyright(c) 2013 Loctronix Corporation
// http://www.loctronix.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#ifndef LIBUSB_EXT_H_
#define LIBUSB_EXT_H_

// Using libusb on linux/osx (via libusb) and CyAPI on windows
#ifdef  __cplusplus
extern "C" {
#endif
#ifdef HAVE_LIBUSB
#include <@LIBUSB_INC_DIR@/libusb.h>
#elif defined(WIN32)
#include <Windows.h>
#include <CyAPI.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIBUSB_EXT_H_ */

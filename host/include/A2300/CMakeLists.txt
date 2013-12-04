##############################################################
# Copyright(c) 2013 Loctronix Corporation
# http://www.loctronix.com
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
##############################################################

##############################################################
# Replace LIBUSB_INC_DIR in this file
##############################################################

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/libusb_ext.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/libusb_ext.h
@ONLY)

##############################################################
# Install information
##############################################################

UHD_INSTALL(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libusb_ext.h
    ${CMAKE_CURRENT_SOURCE_DIR}/A2300InterfaceDefs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/A2300_Defs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/BulkDataPort.h
    ${CMAKE_CURRENT_SOURCE_DIR}/UsbDevice.h
    DESTINATION ${INCLUDE_DIR}/A2300/A2300
    COMPONENT headers
)
